#include "ModelDefault.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
    // スキニング
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        // 座標
        p += (boneWeights[i] * mul(position, _BoneTransforms[boneIndices[i]])).xyz;
        // 法線
        n += (boneWeights[i] * mul(float4(normal.xyz, 0), _BoneTransforms[boneIndices[i]])).xyz;
        // 接線
        t += (boneWeights[i] * mul(float4(tangent.xyz, 0), _BoneTransforms[boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), _viewProjection);
    vout.world_position = p;
    
    vout.normal = normalize(n);
    vout.tangent = normalize(t);
    // 外積をして縦法線を算出
    vout.binormal = cross(vout.tangent, vout.normal);
    
    vout.texcoord = texcoord;
    vout.color = color * _MaterialColor;
    
    return vout;
}
