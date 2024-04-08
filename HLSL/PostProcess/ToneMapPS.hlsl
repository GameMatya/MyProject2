#include "PostProcessDefault.hlsli"
// https://hikita12312.hatenablog.com/entry/2017/08/19/003505

// pow関数に負の値が入る可能性を報告する警告を無効化
#pragma warning (disable : 3571)

Texture2D sceneTexture : register(t0);
Texture2D averageLuminance : register(t1);

#define KeyValue (0.18f)

float3 ACESFilm(float3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    float luminance = averageLuminance.Sample(samplerStates[WRAP_LINEAR], float2(0, 0)).r;
    
    color.rgb = ACESFilm((KeyValue / pow(luminance,1.1)) * color.rgb);
    
    // リニア空間からsRGB空間に変換
    color = pow(color, 1.0 / GAMMA);
    
    return color;
}