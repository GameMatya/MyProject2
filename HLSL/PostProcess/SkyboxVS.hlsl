#include "Skybox.hlsli"

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };

	VS_OUT vout;
    vout.position = float4(position[vertexid], 0, 1);

	//	ƒ[ƒ‹ƒh•ÏŠ·
    float4 p = mul(vout.position, _inverseProjection);
	p /= p.w;
	p = mul(p, _inverseView);
	vout.worldPosition = p.xyz;
	
	return	vout;
}
