#include "../Constants.hlsli"

struct LuminanceExtraction
{
    float threshold;    // ���P�x���o�̂��߂�臒l
    float intensity;    // �u���[���̋��x
    float maxLuminance; // �ő�P�x
    float dummy;
};

struct ColorGrading
{
    float hueShift; // �F������
    float saturation; // �ʓx����
    float brightness; // ���x����
    float dummy;
};

struct DistanceFog
{
    float4 fogColor;
};

cbuffer CbPostProcess : register(b9)
{
    LuminanceExtraction luminanceExtraction;
    
    ColorGrading colorGrading;
    
    DistanceFog distanceFog;
}
