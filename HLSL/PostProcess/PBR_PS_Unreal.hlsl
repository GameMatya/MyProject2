#include "../Unreal_PBR_Functions.hlsli"
#include "../IBL_Functions.hlsli"
#include "../CommonFunctions.hlsli"
#include "../LightData.hlsli"
#include "PostProcessDefault.hlsli"
#include "../ShadowUtility.hlsli"

Texture2D   colorMap        : register(t0);
Texture2D   normalMap       : register(t1); // �@���}�b�v
Texture2D   positionMap     : register(t2); // �ʒu�}�b�v
Texture2D   MRAO_Map        : register(t3); // ���^���b�N���t�l�X���Օ��}�b�v
Texture2D   emissiveMap     : register(t4); // ���Ȕ����}�b�v
Texture2D   depthMap        : register(t5); // �[�x�}�b�v
                                              
Texture2D   skybox          : register(t10); // �X�J�C�{�b�N�X
TextureCube diffuseIem      : register(t11); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Diffuse)
TextureCube specularPmrem   : register(t12); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Specular)
Texture2D   lutGGX          : register(t13); // �X�J�C�{�b�N�X�̐F�Ή��\

static const float MAX_EMISSIVE = 8.0;

float4 main(VS_OUT pin) : SV_TARGET
{
    // �[�x�l���ق�0��������s�N�Z���L��
    float depth = depthMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r;
    clip((1 - depth) - EPSILON);
    
    // �e�N�X�`������p�����[�^�[�擾
    float3 albedoColor = colorMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;
    // ���[���h��Ԃ̖@��
    float3 N = normalMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).xyz;
    // -1 ~ 1 �ɃX�P�[��
    N = normalize(N * 2 - 1);
    // ���[���h���W
    float4 wPos = positionMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    // �����x�E�e���E���Օ�
    float3 MRAO = MRAO_Map.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;
    
    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);
	// �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(dot(Dielectric, Specular * Specular), albedoColor.rgb, MRAO.x);
    
    // ���C�g�x�N�g���A�J�����x�N�g���𐳋K��
    float3 L = normalize(_directionalLight.direction.xyz);
    float3 V = normalize(_cameraPosition.xyz - wPos.xyz);
    
	// ���s�����ɂ��g�U���ˁ{���ʔ���
    float3 directionColor = 0;
    {   
        directionColor = NewUnrealBRDF(-L, _directionalLight.color.rgb, V, N,
        diffuseReflectance, MRAO.x, MRAO.y, F0);
    }
    
    // �V���h�E�C���O����
    [unroll]
    for (int i = 0; i < SHADOW_MAP_COUNT; ++i)
    {
        float3 shadowTexcoord = CalcShadowTexcoord(wPos.xyz, _CascadedLVP[i]);
        
		// �V���h�E�}�b�v��UV�͈͓����A�[�x�l���͈͓������肷��
        if (shadowTexcoord.z > 0 && shadowTexcoord.z < 1 &&
			shadowTexcoord.x > 0 && shadowTexcoord.x < 1 &&
			shadowTexcoord.y > 0 && shadowTexcoord.y < 1)
        {
			// �V���h�E�}�b�v����[�x�l�擾
            float depth = shadowMap[i].Sample(samplerStates[BORDER_POINT], shadowTexcoord.xy).r;
            
			// �[�x�l���r���ĉe���ǂ����𔻒肷��
            if (shadowTexcoord.z - depth > 0.0001)
            {
                // �\�t�g�V���h�E����
                float shadowRate = CalcShadowColorPCFFilter(shadowMap[i], shadowTexcoord, 0.0001);
                directionColor = lerp(directionColor, 0, shadowRate);
            }
            break;
        }
    }
    
    // �����ɂ��g�U���ˁ{���ʔ���
    float3 envColor = 0;
    {
        envColor = IBL(lutGGX, diffuseIem, specularPmrem, samplerStates[BORDER_ANISOTROPIC],
        diffuseReflectance, F0, MRAO.y, N, V);
    }
    
    // �G�~�b�V�u
    float3 emissiveColor = albedoColor.rgb;
    {
        float emissivePower = emissiveMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r * MAX_EMISSIVE;
        emissiveColor *= emissivePower;
    }
    
    float3 color = emissiveColor + directionColor + envColor * MRAO.z;
    
    return float4(color, 1);
}
