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
    // ���W
    float3 p = mul(position, transform).xyz;
    // �@��
    float3 n = mul(float4(normal.xyz, 0), transform).xyz;
    // �ڐ�
    float3 t = mul(float4(tangent.xyz, 0), transform).xyz;
    
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), _viewProjection);
    vout.world_position = p;
    
    vout.normal = normalize(n);
    vout.tangent = normalize(t);
    // �O�ς����ďc�@�����Z�o
    vout.binormal = cross(vout.tangent, vout.normal);
    
    vout.texcoord = texcoord;
    vout.color = color;
    
    return vout;
}

