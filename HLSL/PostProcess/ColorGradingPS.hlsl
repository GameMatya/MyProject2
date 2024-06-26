#include "FilterFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[BORDER_POINT], pin.texcoord);
	// RGB > HSVに変換
    color.rgb = RGB2HSV(color.rgb);

	// 色相調整
    color.r += colorGrading.hueShift;

	// 彩度調整
    color.g *= colorGrading.saturation;

	// 明度調整
    color.b *= colorGrading.brightness;

	// HSV > RGBに変換
    color.rgb = HSV2RGB(color.rgb);
    
    color.a = 1;

    return color;
}
