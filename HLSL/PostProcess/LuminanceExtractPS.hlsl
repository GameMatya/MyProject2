#include "PostProcessDefault.hlsli"
#include "FilterFunctions.hlsli"
//https://hikita12312.hatenablog.com/entry/2017/08/19/003505

Texture2D sceneTexture : register(t0);

// ç≈ëÂãPìx
static const float MAX_LUMINANCE = 2.6;

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 color = sceneTexture.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).rgb;
    
    float luminance = RGB2Luminance(color);
    
    return min(luminance, MAX_LUMINANCE);
}
