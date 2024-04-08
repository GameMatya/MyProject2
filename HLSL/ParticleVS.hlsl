#include "Particle.hlsli"

// 頂点シェーダーでは何もせずジオメトリシェーダーに渡す。
VS_OUT main(in uint vertexId : SV_VERTEXID)
{
    VS_OUT vout;
    vout.vertexId = vertexId;
    return vout;
}