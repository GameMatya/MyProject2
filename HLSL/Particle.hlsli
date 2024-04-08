#include "Constants.hlsli"

static const uint THREAD_NUM_X = 16;

// ���r�R�����̏d��
static const float GRAVITY = 0.68f;

struct VS_OUT
{
    uint vertexId : ID;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct ParticleData
{
    float3  position;
    float   size;
    float   speed;
    float3  moveVector;
    float   gravityAccume;
    float4  color;
    float   emissivePower;
    float   lifeTimer;
    bool    isAlive;
};

cbuffer EmitterData : register(b6)
{
    float3 emitPosition; // �p�[�e�B�N���̐����ʒu
    float gravityModifier; // �d�͂̉e�����󂯂�

    float lifeTime; // �p�[�e�B�N���̐�������
    float emissivePower; // �����̋���
    float spawnRandScale; // �����ʒu�̃����_���̑傫��
    float dummy; // �p�f�B���O

    float3 moveVec; // �i�s����
    float dummy2; // �p�f�B���O
    
    float vecRandScale; // �i�s�����̃����_���̑傫��
    float startSize; // �������̑傫��
    float endSize; // ���Ŏ��̑傫��
    float dummy3; // �p�f�B���O

    float particleSpeed; // �p�[�e�B�N���̑��x
    float speedRandScale; // ���x�̃����_���̑傫��
    float startSpeedRate; // �������̑����̔{��
    float endSpeedRate; // ���Ŏ��̑����̔{��
    
    float4 startColor; // �������̐F
    float4 endColor; // ���Ŏ��̐F
};