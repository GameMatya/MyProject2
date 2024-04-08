#include "Sprite.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    vout.position = float4(position, 1);
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = false;

    return vout;
}
