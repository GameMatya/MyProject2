#include "Constants.hlsli"

#define	KERNEL_MAX 8

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 texcoord[KERNEL_MAX] : TEXCOORD;
};

cbuffer CBGaussianFilter : register(b10)
{
    float4 weights[2];
    
    float kernelSize;
    float2 texel;
    float dummy;
};
