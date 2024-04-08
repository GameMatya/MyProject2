#include "FilterFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[BORDER_POINT], pin.texcoord);
	// RGB > HSV‚É•ÏŠ·
    color.rgb = RGB2HSV(color.rgb);

	// F‘Š’²®
    color.r += colorGrading.hueShift;

	// Ê“x’²®
    color.g *= colorGrading.saturation;

	// –¾“x’²®
    color.b *= colorGrading.brightness;

	// HSV > RGB‚É•ÏŠ·
    color.rgb = HSV2RGB(color.rgb);
    
    color.a = 1;

    return color;
}
