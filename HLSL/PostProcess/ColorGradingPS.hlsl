#include "FilterFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[BORDER_POINT], pin.texcoord);
	// RGB > HSV�ɕϊ�
    color.rgb = RGB2HSV(color.rgb);

	// �F������
    color.r += colorGrading.hueShift;

	// �ʓx����
    color.g *= colorGrading.saturation;

	// ���x����
    color.b *= colorGrading.brightness;

	// HSV > RGB�ɕϊ�
    color.rgb = HSV2RGB(color.rgb);
    
    color.a = 1;

    return color;
}
