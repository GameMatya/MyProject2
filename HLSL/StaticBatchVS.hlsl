#include "StaticBatch.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,

    row_major float4x4 transform : TRANSFORM
)
{   
    // 座標
    float3 p = mul(position, transform).xyz;
    // 法線
    float3 n = mul(float4(normal.xyz, 0), transform).xyz;
    // 接線
    float3 t = mul(float4(tangent.xyz, 0), transform).xyz;
    
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), _viewProjection);
    vout.world_position = p;
    
    vout.normal = normalize(n);
    vout.tangent = normalize(t);
    // 外積をして縦法線を算出
    vout.binormal = cross(vout.tangent, vout.normal);
    
    vout.texcoord = texcoord;
    vout.color = color;
    
    return vout;
}

