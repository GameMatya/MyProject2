#include "Constants.hlsli"

// カスケードシャドウマップの回数
static const int SHADOW_MAP_COUNT = 3;

Texture2D shadowMap[SHADOW_MAP_COUNT] : register(t14); // シャドウマップ

cbuffer ShadowConstants : register(b11)
{
    int _CasterIndex;
    float3 dummy;
    row_major float4x4 _CascadedLVP[SHADOW_MAP_COUNT]; // ライトビュープロジェクション行列
};

// ワールド座標からシャドウマップのサンプル位置を算出
float3 CalcShadowTexcoord(float3 worldPosition, matrix lightViewProjection)
{
	// クリップ空間の座標に変換
    float4 worldViewProjectionPosition = mul(float4(worldPosition, 1), lightViewProjection);

	// 透視除算してNDC座標に変換
    worldViewProjectionPosition /= worldViewProjectionPosition.w;

	// NDC座標のXY座標をUV座標に変換    
    worldViewProjectionPosition.y *= -1;
    // 大きさを調整する
    worldViewProjectionPosition.xy = (worldViewProjectionPosition.xy + 1) / 2;
    
    return worldViewProjectionPosition.xyz;
}

static const int PCF_KERNEL_SIZE = 5; //	指定は奇数にすること
float CalcShadowColorPCFFilter(Texture2D tex, float3 shadowTexcoord, float shadowBias)
{
	// テクセルサイズの計算
    float2 texelSize;
	{
		// テクスチャの縦幅横幅を取得する
        int width, height;
        tex.GetDimensions(width, height);

		// 算出
        texelSize = float2(1.0f / width, 1.0f / height);
    }

    float factor = 0;
    [unroll]
    for (int x = -PCF_KERNEL_SIZE / 2; x <= PCF_KERNEL_SIZE / 2; ++x)
    {
        [unroll]
        for (int y = -PCF_KERNEL_SIZE / 2; y <= PCF_KERNEL_SIZE / 2; ++y)
        {
			// シャドウマップから深度値取得
            float depth = tex.Sample(samplerStates[BORDER_ANISOTROPIC], shadowTexcoord.xy + texelSize * float2(x, y)).r;
            factor += step(shadowBias, shadowTexcoord.z - depth);
        }
    }
	// 深度値を比較して影かどうかを判定する
    return factor / (PCF_KERNEL_SIZE * PCF_KERNEL_SIZE);
}
