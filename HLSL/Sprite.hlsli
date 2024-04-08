#include "Constants.hlsli"

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
    bool   isLinear : IS_LINEAR;
};

