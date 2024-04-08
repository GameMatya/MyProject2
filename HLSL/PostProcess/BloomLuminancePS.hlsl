#include "FilterFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[BORDER_LINEAR], pin.texcoord);

	// RGB > �P�x�l�ɕϊ�
    float luminance = RGB2Luminance(color.rgb);
    clip(luminanceExtraction.maxLuminance - luminance);

	// 臒l�Ƃ̍����Z�o
    float contribution = luminance - luminanceExtraction.threshold;
    clip(contribution);
    
    return float4(color.rgb * contribution * luminanceExtraction.intensity, color.a);
}
