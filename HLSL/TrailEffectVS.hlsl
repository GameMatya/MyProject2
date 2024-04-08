#include "Sprite.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    vout.position = mul(float4(position, 1), mul(_view, _projection));
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = true;

    return vout;
}
