#include "../PBR_Functions.hlsli"
#include "../IBL_Functions.hlsli"
#include "../CommonFunctions.hlsli"
#include "../LightData.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1); // �@���}�b�v
Texture2D positionMap : register(t2); // �ʒu�}�b�v
Texture2D MRAO_Map : register(t3); // ���^���b�N���t�l�X���Օ��}�b�v
Texture2D depthMap : register(t4); // �[�x�}�b�v

Texture2D skyboxMap : register(t10); // ��(����)�e�N�X�`��

float4 main(VS_OUT pin) : SV_TARGET
{
    // �[�x�l���ق�0��������s�N�Z���L��
    float depth = depthMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r;
    clip((1 - depth) - 0.000001);
    
    // �e�N�X�`������p�����[�^�[�擾
    float4 albedoColor = colorMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    float3 N = normalMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).xyz;
    // UV����R�����ɕϊ�
    N = N * 2 - 1;
    float4 wPos = positionMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    // �����x�E�e���E���Օ�
    float3 MRAO = MRAO_Map.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;

    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);
	// �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(Dielectric, albedoColor.rgb, MRAO.x);
    
    // ���C�g�x�N�g���A�J�����x�N�g���𐳋K��
    float3 L = normalize(_directionalLight.direction.xyz);
    float3 E = normalize(_cameraPosition.xyz - wPos.xyz);

	// ���C�e�B���O�v�Z
    float3 directDiffuse = 0, directSpecular = 0;
    {   
        // ���s�����̏���
        DirectBDRF(diffuseReflectance, F0, N, E, -L,
				   _directionalLight.color.rgb, MRAO.y,
				   directDiffuse, directSpecular);
    }
    return float4(directDiffuse + directSpecular, 1);
    
 //   // �Ԑڌ��̊g�U���ːF
 //   float3 irradiance = CalcHemisphereLight(N, hemisphereLightData) + ambientLightColor.rgb;
 //   float3 indirectDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
    
 //   // �Ԑڌ��̋��ʔ��ːF
 //   float3 indirectSpecular = 0;
	//{
	//	// �X�J�C�{�b�N�X����F���擾
 //       float2 texcoord = EquirectangularProjection(reflect(-E, N));
 //       // �e�N�X�`������~�b�v�}�b�v�̏����擾����
 //       uint width, height, mipMapLevel;
 //       skyboxMap.GetDimensions(0, width, height, mipMapLevel);
 //       float mipLevel = lerp(0, mipMapLevel - 1, MRAO.y);
 //       float3 raddiance = skyboxMap.SampleLevel(samplerStates[BORDER_ANISOTROPIC], texcoord, mipLevel).rgb;
 //       raddiance = pow(raddiance, 2.2);
        
 //       indirectSpecular = EnvBRDFApprox(F0, MRAO.y, dot(N, E)) * raddiance;
 //   }
    
 //   float3 color = directDiffuse + directSpecular +
 //   (indirectDiffuse + indirectSpecular) * MRAO.z;
    
 //   return float4(color, 1);
}
