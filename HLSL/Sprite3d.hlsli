#include "Sprite.hlsli"

cbuffer Sprite3D : register(b0)
{
    row_major float4x4 worldMatrix;
    
    int4 billboardAxis; // w = padding
}
