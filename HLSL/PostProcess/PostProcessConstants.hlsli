#include "../Constants.hlsli"

struct LuminanceExtraction
{
    float threshold;    // 高輝度抽出のための閾値
    float intensity;    // ブルームの強度
    float maxLuminance; // 最大輝度
    float dummy;
};

struct ColorGrading
{
    float hueShift; // 色相調整
    float saturation; // 彩度調整
    float brightness; // 明度調整
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
