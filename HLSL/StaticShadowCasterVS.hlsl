#include "Constants.hlsli"
#include "ShadowUtility.hlsli"

float4 main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,

    row_major float4x4 transform : TRANSFORM
) : SV_POSITION
{
    // ç¿ïW
    float3 p = mul(position, transform).xyz;
    
    return mul(float4(p, 1.0f), _CascadedLVP[_CasterIndex]);
}

