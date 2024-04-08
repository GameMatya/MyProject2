#include "PostProcessDefault.hlsli"

Texture2D averageLuminance : register(t0);
Texture2D oldAverageLuminance : register(t1);

// �x���
#define DelayVolume (15)

// ���ϋP�x�̍X�V��x��������
float4 main(VS_OUT pin) : SV_TARGET
{
    float current = averageLuminance.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    float old = oldAverageLuminance.Sample(samplerStates[WRAP_LINEAR], pin.texcoord).r;
    old *= DelayVolume;
    
    return (current + old) / (DelayVolume + 1);
}