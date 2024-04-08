#include "PostProcessDefault.hlsli"
//https://hikita12312.hatenablog.com/entry/2017/08/19/003505

Texture2D texture_map : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 size;
    float level;
    texture_map.GetDimensions(0, size.x, size.y, level);
    
    const float2 tecel = float2(1 / size.x, 1 / size.y);
    
    float4 l0 = texture_map.Sample(samplerStates[WRAP_LINEAR], pin.texcoord - float2(tecel.x, 0));
    float4 l1 = texture_map.Sample(samplerStates[WRAP_LINEAR], pin.texcoord - float2(0, tecel.y));
    float4 l2 = texture_map.Sample(samplerStates[WRAP_LINEAR], pin.texcoord + float2(tecel.x, 0));
    float4 l3 = texture_map.Sample(samplerStates[WRAP_LINEAR], pin.texcoord + float2(0, tecel.y));

    float total = l0.r + l1.r + l2.r + l3.r;

    return total / 4;
}