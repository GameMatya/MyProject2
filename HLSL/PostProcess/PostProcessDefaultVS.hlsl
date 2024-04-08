#include "PostProcessDefault.hlsli"

static const float2 POSITION[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
static const float2 TEXCOORD[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout;
    vout.position = float4(POSITION[vertexid], 0, 1);
    vout.texcoord = TEXCOORD[vertexid];
    
    return vout;
}