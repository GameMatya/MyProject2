#include "Constants.hlsli"
#include "Postprocess/FilterFunctions.hlsli"

// 参考サイト
// https://qiita.com/emadurandal/items/b2ae09c5cc1b3da821c8
// http://project-asura.com/blog/archives/3124

float4 sampleLutGGX(Texture2D lutGGX, SamplerState sampleState, float2 brdf_sample_point)
{
    return lutGGX.Sample(sampleState, brdf_sample_point);
}

float4 sampleDiffuseIem(TextureCube diffuseIem, SamplerState sampleState, float3 v)
{
    return diffuseIem.Sample(sampleState, v);
}

float4 sampleSpecularPmrem(TextureCube specularPmrem, SamplerState sampleState, float3 v, float roughness)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);

    float lod = roughness * float(numberOfLevels - 1);

    return specularPmrem.SampleLevel(sampleState, v, lod);
}

float3 ApproximateSpecularIBL(Texture2D lutGGX, TextureCube specularPmrem, SamplerState sampleState, float3 F0, float Roughness, float3 N, float3 V)
{
    float NdotV = saturate(dot(N, V));
    float3 R = normalize(reflect(-V, N));
    float2 samplePoint = saturate(float2(NdotV, Roughness));
    
    // プレフィルタリングされた入射光（スペキュラIBLテクスチャ）
    float3 PrefilteredColor = sampleSpecularPmrem(specularPmrem, sampleState, R, Roughness).rgb;

    // Environment BRDF 2D LUT
    float2 EnvBRDF = sampleLutGGX(lutGGX, sampleState, samplePoint).xy;
    
    // F0 = SpecularColor
    return PrefilteredColor * (F0 * EnvBRDF.x + EnvBRDF.y);
}

float3 RadianceLambertian(Texture2D lutGGX, TextureCube diffuseIem, SamplerState sampleState, float3 N, float3 V, float roughness, float3 diffuse_color, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);

    float2 samplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = sampleLutGGX(lutGGX, sampleState, samplePoint).rg;

    float3 irradiance = sampleDiffuseIem(diffuseIem, sampleState, N).rgb;

    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;

    float ems = (1.0 - (f_ab.x + f_ab.y));
    float3 f_avg = (f0 + (1.0 - f0) / 21.0);
    float3 fms_ems = ems * fss_ess * f_avg / (1.0 - f_avg * ems);
    float3 k_d = diffuse_color * (1.0 - fss_ess + fms_ems);

    return (fms_ems + k_d) * irradiance;
}

#define MaxLuminance (2)

float3 IBL(Texture2D lutGGX, TextureCube diffuseIem, TextureCube specularPmrem, SamplerState sampleState, float3 DiffuseColor, float3 F0, float Roughness, float3 N, float3 V)
{
    float3 diffuse = RadianceLambertian(lutGGX, diffuseIem, sampleState, N, V, Roughness, DiffuseColor, F0);
    float3 specular = ApproximateSpecularIBL(lutGGX, specularPmrem, sampleState, F0, Roughness, N, V);
    
    // スペキュラ反射の強さを調整
    float LuminanceRatio = MaxLuminance / RGB2Luminance(specular);
    LuminanceRatio = clamp(LuminanceRatio, 0.0, 1.0);
    
    return diffuse + specular * LuminanceRatio;
}