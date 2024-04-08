#include "Sprite3d.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    
    vout.position = mul(mul(float4(position, 1), worldMatrix), _viewProjection);
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = true;

    return vout;
}
