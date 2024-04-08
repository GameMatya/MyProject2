// ���ƃx�[�X�̕����x�[�X�����_�����O
#include "Constants.hlsli"

//--------------------------------------------
// �Ԑڌ���BRDF
//--------------------------------------------
// F0			: �������ˎ��̃t���l�����ːF
// roughness	: �e��
// NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
float3 EnvBRDFApprox(float3 F0, float roughness, float NdotV)
{
    const float4 c0 = { -1.0f, -0.0275f, -0.572f, 0.022f };
    const float4 c1 = { 1.0f, 0.0425f, 1.04f, -0.04f };
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28f * NdotV)) * r.x + r.y;
    float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
    return F0 * AB.x + AB.y;
}

// �U�d��(������ł��Œ�4%(0.04f)�͋��ʔ��˂���)
static const float Dielectric = 0.04f;
//--------------------------------------------
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
// diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// D�� : �@�����z�֐�
//--------------------------------------------
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
// roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a2 = pow(roughness, 2);

    return a2 / (PI * pow(pow(NdotH, 2) * (a2 - 1) + 1, 2));
}
//--------------------------------------------
// G�� : �􉽌������̎Z�o
//--------------------------------------------
// Ndotv : �@���Ƒ��x�N�g���Ƃ̓���
// k : �e��/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / (Ndotv * (1 - k) + k);
}
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// F�� : �t���l����
//--------------------------------------------
// F0 : �������ˎ��̔��˗�
// VdotH: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1 - F0) * pow(1 - VdotH, 5);
}
//--------------------------------------------
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
// NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// fresnelF0	: �������ˎ��̃t���l�����ːF
// roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	// D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	// G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	// F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return (D * G * F) / (4 * NdotL * NdotV);

}
//--------------------------------------------
//	���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
// diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
// F0					: �������ˎ��̃t���l�����ːF
// normal				: �@���x�N�g��(���K���ς�)
// eyeVector			: ���_�Ɍ������x�N�g��(���K���ς�)
// lightVector			: �����x�N�g��(���K���ς�)
// lightColor			: ���C�g�J���[
// roughness			: �e��
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 normal,
				float3 eyeVector,
				float3 lightVector,
				float3 lightColor,
				float roughness,
				out float3 outDiffuse,
				out float3 outSpecular)
{
    float3 N = normal;
    float3 L = lightVector;
    float3 V = eyeVector;
    float3 H = normalize(L + V);

    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));

    float3 irradiance = lightColor * NdotL;

	// ���K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂꍇ��PI�ŏ�Z����
    irradiance *= PI;

	// �g�U����BRDF(���K�������o�[�g�̊g�U����)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;

	// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}
