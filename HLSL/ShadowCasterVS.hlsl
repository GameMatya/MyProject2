#include "ModelConstants.hlsli"
#include "ShadowUtility.hlsli"

float4 main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
) : SV_POSITION
{
    // スキニング
    float3 p = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        // 座標
        p += (boneWeights[i] * mul(position, _BoneTransforms[boneIndices[i]])).xyz;
    }
    
    return mul(float4(p, 1.0f), _CascadedLVP[_CasterIndex]);
}
