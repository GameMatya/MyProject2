#include "Skybox.hlsli"
#include "FilterFunctions.hlsli"

// pow関数に負の値が入る可能性を報告する警告を無効化
#pragma warning (disable : 3571)

Texture2D skybox : register(t10);

float4 main(VS_OUT pin) : SV_TARGET
{
	// カメラからワールド座標へのベクトル
    float3 V = normalize(pin.worldPosition.xyz - _cameraPosition.xyz);
	
    float2 sample_point;
	
	// atan2(z,x) = xz平面上の方向をラジアン単位で取得 ( -PI ~ PI )
	// + PI = 正の値のみにする ( 0 ~ 2PI )
	// ÷ 2PI = UV値に変換 ( 0 ~ 1 )
    sample_point.x = (atan2(V.z, V.x) + PI) / (PI * 2.0);
	
	// asin(y) = yの逆正弦をラジアン単位で取得 ( -PI/2 ~ PI/2 )
	// + PI/2 = 正の値のみにする ( 0 ~ PI )
	// ÷ PI = UV値に変換 ( 0 ~ 1 )
    sample_point.y = ((asin(V.y) + PI * 0.5) / PI);
	// 1 - point.y = UVに合わせる為、yの値を反転
    sample_point.y = 1 - sample_point.y;

    float3 color = skybox.SampleLevel(samplerStates[WRAP_ANISOTROPIC], sample_point, 0).rgb;
	// リニア色空間に変換 (トーンマッピングでsRGB色空間に変換される)
    color = pow(color, GAMMA);
	
	// 輝度を求める
    float luminance = RGB2Luminance(color);
	
	// 輝度が小さいほどColorを大きくしていく ( デフォルトだと太陽と空で輝度の差が大きすぎるため調整 )
    const float LUMINANCE_OFFSET = 0.1f;
    color += color * (LUMINANCE_OFFSET / luminance);
	
    return float4(color, 1);
}
