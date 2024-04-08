#include "Constants.hlsli"

// �J�X�P�[�h�V���h�E�}�b�v�̉�
static const int SHADOW_MAP_COUNT = 3;

Texture2D shadowMap[SHADOW_MAP_COUNT] : register(t14); // �V���h�E�}�b�v

cbuffer ShadowConstants : register(b11)
{
    int _CasterIndex;
    float3 dummy;
    row_major float4x4 _CascadedLVP[SHADOW_MAP_COUNT]; // ���C�g�r���[�v���W�F�N�V�����s��
};

// ���[���h���W����V���h�E�}�b�v�̃T���v���ʒu���Z�o
float3 CalcShadowTexcoord(float3 worldPosition, matrix lightViewProjection)
{
	// �N���b�v��Ԃ̍��W�ɕϊ�
    float4 worldViewProjectionPosition = mul(float4(worldPosition, 1), lightViewProjection);

	// �������Z����NDC���W�ɕϊ�
    worldViewProjectionPosition /= worldViewProjectionPosition.w;

	// NDC���W��XY���W��UV���W�ɕϊ�    
    worldViewProjectionPosition.y *= -1;
    // �傫���𒲐�����
    worldViewProjectionPosition.xy = (worldViewProjectionPosition.xy + 1) / 2;
    
    return worldViewProjectionPosition.xyz;
}

static const int PCF_KERNEL_SIZE = 5; //	�w��͊�ɂ��邱��
float CalcShadowColorPCFFilter(Texture2D tex, float3 shadowTexcoord, float shadowBias)
{
	// �e�N�Z���T�C�Y�̌v�Z
    float2 texelSize;
	{
		// �e�N�X�`���̏c���������擾����
        int width, height;
        tex.GetDimensions(width, height);

		// �Z�o
        texelSize = float2(1.0f / width, 1.0f / height);
    }

    float factor = 0;
    [unroll]
    for (int x = -PCF_KERNEL_SIZE / 2; x <= PCF_KERNEL_SIZE / 2; ++x)
    {
        [unroll]
        for (int y = -PCF_KERNEL_SIZE / 2; y <= PCF_KERNEL_SIZE / 2; ++y)
        {
			// �V���h�E�}�b�v����[�x�l�擾
            float depth = tex.Sample(samplerStates[BORDER_ANISOTROPIC], shadowTexcoord.xy + texelSize * float2(x, y)).r;
            factor += step(shadowBias, shadowTexcoord.z - depth);
        }
    }
	// �[�x�l���r���ĉe���ǂ����𔻒肷��
    return factor / (PCF_KERNEL_SIZE * PCF_KERNEL_SIZE);
}
