#include "ModelConstants.hlsli"
#include "ShadowUtility.hlsli"

[earlydepthstencil]
float4 main(float4 svPosition : SV_POSITION) : SV_TARGET
{
    float depth = svPosition.z;
    return float4(depth, 0, 0, 1);
}

