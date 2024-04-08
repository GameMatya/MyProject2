#include "GaussianBlur.hlsli"

static const float2 POSITION[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
static const float2 TEXCOORD[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout = (VS_OUT)0;
    vout.position = float4(POSITION[vertexid], 0, 1);
    
    // ��e�N�Z����UV���W���L�^
    float2 tex = TEXCOORD[vertexid];

    // ��e�N�Z������U���W�����炷���߂̃I�t�Z�b�g���v�Z����
    [unroll]
    for (int i = 0; i < kernelSize; ++i)
    {
        vout.texcoord[i].xy = float2(texel.x * (1 + (i * 2)), 0.0f);

        // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
        vout.texcoord[i].zw = vout.texcoord[i].xy * -1;

        // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
        // ���ۂɃT���v�����O����UV���W�ɕϊ�����
        vout.texcoord[i] += float4(tex, tex);
    }
    
    return vout;
}
