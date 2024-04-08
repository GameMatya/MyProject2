#include "ModelDefault.hlsli"

// pow関数に負の値が入る可能性を報告する警告を無効化
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
    // 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
    float3 normal = normalMap.Sample(samplerStates[WRAP_ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;
    
	// 変換用の3X3行列を用意
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };
	// 法線マップで取得した法線情報に変換行列を掛け合わせる
    float3 N = normalize(mul(normal, CM));
    // ワールド空間の法線( 0 ～ 1 )の間にスケーリング
    N = (N * 0.5) + 0.5;
        
    PS_OUT pout = (PS_OUT) 0;
    pout.color = diffuseMap.Sample(samplerStates[WRAP_ANISOTROPIC], pin.texcoord) * pin.color;
    // リニア空間に変換
    pout.color.rgb = pow(pout.color.rgb, GAMMA);

    pout.normal = float4(N, 1);
    pout.position = float4(pin.world_position, 1);
    
    pout.M_R_AO = 0;
    pout.M_R_AO.a = 1;
    
    // メタリック
    pout.M_R_AO.r = metallMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    // ラフネス
    pout.M_R_AO.g = roughnessMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    // アンビエント
    pout.M_R_AO.b = ambientOcclusionMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    
    // エミッシブ
    pout.emissive.r = emissiveMap.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    pout.emissive.a = 1.0;
    
    return pout;
}

