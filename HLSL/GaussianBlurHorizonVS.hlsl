#include "GaussianBlur.hlsli"

static const float2 POSITION[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
static const float2 TEXCOORD[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout = (VS_OUT)0;
    vout.position = float4(POSITION[vertexid], 0, 1);
    
    // 基準テクセルのUV座標を記録
    float2 tex = TEXCOORD[vertexid];

    // 基準テクセルからU座標をずらすためのオフセットを計算する
    [unroll]
    for (int i = 0; i < kernelSize; ++i)
    {
        vout.texcoord[i].xy = float2(texel.x * (1 + (i * 2)), 0.0f);

        // オフセットに-1を掛けてマイナス方向のオフセットも計算する
        vout.texcoord[i].zw = vout.texcoord[i].xy * -1;

        // オフセットに基準テクセルのUV座標を足し算して、
        // 実際にサンプリングするUV座標に変換する
        vout.texcoord[i] += float4(tex, tex);
    }
    
    return vout;
}
