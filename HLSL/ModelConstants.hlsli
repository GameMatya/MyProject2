#include "Constants.hlsli"

#define MAX_BONES 160
cbuffer CbMesh : register(b6)
{
    row_major float4x4 _BoneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b7)
{
    float4 _MaterialColor;
};
