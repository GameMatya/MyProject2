#include "ModelDefault.hlsli"

// pow�֐��ɕ��̒l������\����񍐂���x���𖳌���
#pragma warning (disable : 3571)

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 position : SV_TARGET2;
    float4 M_R_AO : SV_TARGET3; // x=metallic, y=smoothness, z=ambientOcclusion
    float4 emissive : SV_TARGET4;
};

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D metallMap : register(t2);
Texture2D roughnessMap : register(t3);
Texture2D ambientOcclusionMap : register(t4);
Texture2D emissiveMap : register(t5);

Texture2D shadowMap : register(t14);

[earlydepthstencil]
PS_OUT main(VS_OUT pin)
{
    // �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
    float3 normal = normalMap.Sample(samplerStates[WRAP_ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;
    
	// �ϊ��p��3X3�s���p��
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };
	// �@���}�b�v�Ŏ擾�����@�����ɕϊ��s����|�����킹��
    float3 N = normalize(mul(normal, CM));
    // ���[���h��Ԃ̖@��( 0 �` 1 )�̊ԂɃX�P�[�����O
    N = (N * 0.5) + 0.5;
        
    PS_OUT pout = (PS_OUT) 0;
    pout.color = diffuseMap.Sample(samplerStates[WRAP_ANISOTROPIC], pin.texcoord) * pin.color;
    // ���j�A��Ԃɕϊ�
    pout.color.rgb = pow(pout.color.rgb, GAMMA);

    pout.normal = float4(N, 1);
    pout.position = float4(pin.world_position, 1);
    
    pout.M_R_AO = 0;
    pout.M_R_AO.a = 1;
    
    // ���^���b�N
    pout.M_R_AO.r = metallMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    // ���t�l�X
    pout.M_R_AO.g = roughnessMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    // �A���r�G���g
    pout.M_R_AO.b = ambientOcclusionMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    
    // �G�~�b�V�u
    pout.emissive.r = emissiveMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    pout.emissive.a = 1.0;
    
    return pout;
}

