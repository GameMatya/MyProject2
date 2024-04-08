#include "FilterFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[BORDER_LINEAR], pin.texcoord);

	// RGB > 輝度値に変換
    float luminance = RGB2Luminance(color.rgb);
    clip(luminanceExtraction.maxLuminance - luminance);

	// 閾値との差を算出
    float contribution = luminance - luminanceExtraction.threshold;
    clip(contribution);
    
    return float4(color.rgb * contribution * luminanceExtraction.intensity, color.a);
}
