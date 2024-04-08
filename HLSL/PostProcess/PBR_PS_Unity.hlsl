#include "../PBR_Functions.hlsli"
#include "../IBL_Functions.hlsli"
#include "../CommonFunctions.hlsli"
#include "../LightData.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1); // 法線マップ
Texture2D positionMap : register(t2); // 位置マップ
Texture2D MRAO_Map : register(t3); // メタリックラフネス環境遮蔽マップ
Texture2D depthMap : register(t4); // 深度マップ

Texture2D skyboxMap : register(t10); // 空(そら)テクスチャ

float4 main(VS_OUT pin) : SV_TARGET
{
    // 深度値がほぼ0だったらピクセルキル
    float depth = depthMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r;
    clip((1 - depth) - 0.000001);
    
    // テクスチャからパラメーター取得
    float4 albedoColor = colorMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    float3 N = normalMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).xyz;
    // UVから３次元に変換
    N = N * 2 - 1;
    float4 wPos = positionMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    // 金属度・粗さ・環境遮蔽
    float3 MRAO = MRAO_Map.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;

    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);
	// 垂直反射時のフレネル反射率
    float3 F0 = lerp(Dielectric, albedoColor.rgb, MRAO.x);
    
    // ライトベクトル、カメラベクトルを正規化
    float3 L = normalize(_directionalLight.direction.xyz);
    float3 E = normalize(_cameraPosition.xyz - wPos.xyz);

	// ライティング計算
    float3 directDiffuse = 0, directSpecular = 0;
    {   
        // 平行光源の処理
        DirectBDRF(diffuseReflectance, F0, N, E, -L,
				   _directionalLight.color.rgb, MRAO.y,
				   directDiffuse, directSpecular);
    }
    return float4(directDiffuse + directSpecular, 1);
    
 //   // 間接光の拡散反射色
 //   float3 irradiance = CalcHemisphereLight(N, hemisphereLightData) + ambientLightColor.rgb;
 //   float3 indirectDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;
    
 //   // 間接光の鏡面反射色
 //   float3 indirectSpecular = 0;
	//{
	//	// スカイボックスから色を取得
 //       float2 texcoord = EquirectangularProjection(reflect(-E, N));
 //       // テクスチャからミップマップの情報を取得する
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
