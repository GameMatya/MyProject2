#include "Particle.hlsli"

// ���_�V�F�[�_�[�ł͉��������W�I���g���V�F�[�_�[�ɓn���B
VS_OUT main(in uint vertexId : SV_VERTEXID)
{
    VS_OUT vout;
    vout.vertexId = vertexId;
    return vout;
}