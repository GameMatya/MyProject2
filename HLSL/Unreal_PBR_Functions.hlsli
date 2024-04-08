#include "Constants.hlsli"

// 参考サイト
// http://project-asura.com/blog/archives/3124
// https://qiita.com/emadurandal/items/3a8db7bc61438245654d
// https://light11.hatenadiary.com/entry/2020/03/05/220957
// https://rayspace.xyz/CG/contents/Disney_principled_BRDF/

// 誘電率
static const float Dielectric = 0.16f;
static const float Specular = 0.5f;

//------------------------------------------------
//	 拡散反射BRDF
//------------------------------------------------
// 正規化ランバート
float3 DiffuseBRDF(float3 albedo)
{
    return albedo / PI;
}


//------------------------------------------------
//	 鏡面反射BRDF
//------------------------------------------------
// 法線分布関数 D項 ( GGX/Trowbridge-Reitz )
#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

inline float D_GGX(float NdotH, float3 NxH, float roughness)
{
    float a = NdotH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

// 幾何減衰項 G項 ( Schlick Ver Disney )
#define G1Func(_dot) (_dot /(_dot * (1 - k)+ k)) 

inline float G_SchlickGGX(float NdotL, float NdotV, float roughness)
{
    float k = (roughness + 1) * (roughness + 1) / 8;
    return G1Func(NdotL) * G1Func(NdotV);
}

// 幾何減衰項 V項 ( Smith Joint GGX 近似式 ) "鏡面反射BRDFの式が変わるので注意"
inline float V_HeightCorrelatedGGX(float NdotL, float NdotV, float roughness)
{
    float alpha = roughness * roughness;
    float lambdaV = NdotL * (NdotV * (1 - alpha) + alpha);
    float lambdaL = NdotV * (NdotL * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

// フレネル項 F項 ( Schlick )
inline float3 F_Schlick(float3 f0, float EdotH)
{
    return f0 + (1 - f0) * pow(1 - EdotH, 5);
}

// フレネル項 F項 ( Spherical Gaussian : Schlickの近似 )
inline float3 F_SphericalGaussian(float3 f0, float EdotH)
{
    return f0 + (1 - f0) * exp2((-5.55473 * EdotH - 6.98316) * EdotH);
}

// 鏡面反射BRDF (DGF項Ver)
inline float3 OldSpecularBRDF(float3 harfV, float NdotL, float NdotV, float NdotH, float3 NxH, float rough, float3 f0)
{
    float3 DGF = D_GGX(NdotH, NxH, rough) * G_SchlickGGX(NdotL, NdotV, rough) * F_SphericalGaussian(f0, NdotH);
    return DGF / (4 * NdotL * NdotV);
}

// 鏡面反射BRDF (DVF項Ver)
inline float3 NewSpecularBRDF(float3 harfV, float NdotL, float NdotV, float NdotH, float3 NxH, float rough, float3 f0)
{
    return D_GGX(NdotH, NxH, rough) * V_HeightCorrelatedGGX(NdotL, NdotV, rough) * F_Schlick(f0, NdotH);
}


//------------------------------------------------
//	 PBR_BRDF
//------------------------------------------------
inline float3 OldUnrealBRDF(float3 lightVec, float3 lightColor, float3 view, float3 normal, float3 albedo, float metal, float rough, float3 f0)
{
    float3 harfV = normalize(lightVec + view);
    float NdotL = max(dot(normal, lightVec), 0.0001);
    float NdotV = max(dot(normal, view), 0.0001);
    float NdotH = max(dot(normal, harfV), 0.0001);
    float3 NxH = cross(normal, harfV);
    
    float3 irradiance = lightColor * NdotL;
    float3 Fr = DiffuseBRDF(albedo) * (1 - metal) + OldSpecularBRDF(harfV, NdotL, NdotV, NdotH, NxH, rough, f0);
    
    return Fr * irradiance;
}

inline float3 NewUnrealBRDF(float3 lightVec, float3 lightColor, float3 view, float3 normal, float3 albedo, float metal, float rough, float3 f0)
{
    float3 harfV = normalize(lightVec + view);
    float NdotL = max(dot(normal, lightVec), 0.0001);
    float NdotV = max(dot(normal, view), 0.0001);
    float NdotH = max(dot(normal, harfV), 0.0001);
    float3 NxH = cross(normal, harfV);
    
    float3 irradiance = lightColor * NdotL;
    float3 Fr = DiffuseBRDF(albedo) * (1 - metal) + NewSpecularBRDF(harfV, NdotL, NdotV, NdotH, NxH, rough, f0);
    
    return Fr * irradiance;
}
