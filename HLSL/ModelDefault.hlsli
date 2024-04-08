#include "ModelConstants.hlsli"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT; // ê⁄ê¸
    float3 binormal : BINORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};
