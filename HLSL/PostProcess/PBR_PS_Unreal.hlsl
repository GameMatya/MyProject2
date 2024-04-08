#include "../Unreal_PBR_Functions.hlsli"
#include "../IBL_Functions.hlsli"
#include "../CommonFunctions.hlsli"
#include "../LightData.hlsli"
#include "PostProcessDefault.hlsli"
#include "../ShadowUtility.hlsli"

Texture2D   colorMap        : register(t0);
Texture2D   normalMap       : register(t1); // 法線マップ
Texture2D   positionMap     : register(t2); // 位置マップ
Texture2D   MRAO_Map        : register(t3); // メタリックラフネス環境遮蔽マップ
Texture2D   emissiveMap     : register(t4); // 自己発光マップ
Texture2D   depthMap        : register(t5); // 深度マップ
                                              
Texture2D   skybox          : register(t10); // スカイボックス
TextureCube diffuseIem      : register(t11); // プレフィルタリング済スカイボックス(Diffuse)
TextureCube specularPmrem   : register(t12); // プレフィルタリング済スカイボックス(Specular)
Texture2D   lutGGX          : register(t13); // スカイボックスの色対応表

static const float MAX_EMISSIVE = 8.0;

float4 main(VS_OUT pin) : SV_TARGET
{
    // 深度値がほぼ0だったらピクセルキル
    float depth = depthMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r;
    clip((1 - depth) - EPSILON);
    
    // テクスチャからパラメーター取得
    float3 albedoColor = colorMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;
    // ワールド空間の法線
    float3 N = normalMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).xyz;
    // -1 ~ 1 にスケール
    N = normalize(N * 2 - 1);
    // ワールド座標
    float4 wPos = positionMap.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    // 金属度・粗さ・環境遮蔽
    float3 MRAO = MRAO_Map.Sample(samplerStates[BORDER_POINT], pin.texcoord).rgb;
    
    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);
	// 垂直反射時のフレネル反射率
    float3 F0 = lerp(dot(Dielectric, Specular * Specular), albedoColor.rgb, MRAO.x);
    
    // ライトベクトル、カメラベクトルを正規化
    float3 L = normalize(_directionalLight.direction.xyz);
    float3 V = normalize(_cameraPosition.xyz - wPos.xyz);
    
	// 平行光源による拡散反射＋鏡面反射
    float3 directionColor = 0;
    {   
        directionColor = NewUnrealBRDF(-L, _directionalLight.color.rgb, V, N,
        diffuseReflectance, MRAO.x, MRAO.y, F0);
    }
    
    // シャドウイング処理
    [unroll]
    for (int i = 0; i < SHADOW_MAP_COUNT; ++i)
    {
        float3 shadowTexcoord = CalcShadowTexcoord(wPos.xyz, _CascadedLVP[i]);
        
		// シャドウマップのUV範囲内か、深度値が範囲内か判定する
        if (shadowTexcoord.z > 0 && shadowTexcoord.z < 1 &&
			shadowTexcoord.x > 0 && shadowTexcoord.x < 1 &&
			shadowTexcoord.y > 0 && shadowTexcoord.y < 1)
        {
			// シャドウマップから深度値取得
            float depth = shadowMap[i].Sample(samplerStates[BORDER_POINT], shadowTexcoord.xy).r;
            
			// 深度値を比較して影かどうかを判定する
            if (shadowTexcoord.z - depth > 0.0001)
            {
                // ソフトシャドウ処理
                float shadowRate = CalcShadowColorPCFFilter(shadowMap[i], shadowTexcoord, 0.0001);
                directionColor = lerp(directionColor, 0, shadowRate);
            }
            break;
        }
    }
    
    // 環境光による拡散反射＋鏡面反射
    float3 envColor = 0;
    {
        envColor = IBL(lutGGX, diffuseIem, specularPmrem, samplerStates[BORDER_ANISOTROPIC],
        diffuseReflectance, F0, MRAO.y, N, V);
    }
    
    // エミッシブ
    float3 emissiveColor = albedoColor.rgb;
    {
        float emissivePower = emissiveMap.Sample(samplerStates[BORDER_POINT], pin.texcoord).r * MAX_EMISSIVE;
        emissiveColor *= emissivePower;
    }
    
    float3 color = emissiveColor + directionColor + envColor * MRAO.z;
    
    return float4(color, 1);
}
