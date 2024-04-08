#include "NaturalVignetting.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    const float2 d = abs(pin.texcoord - float2(0.5f, 0.5f)) * float2(2.0f * ASPECT_RATIO, 2.0f);
    const float r2 = dot(d, d);
    float vignettingFactor = 0.0f;

    // 口径食の効果
    vignettingFactor += pow(min(1.0f, r2 / VIGNETTING_MECHANICAL_RADIUS2), VIGNETTING_MECHANICAL_INV_SMOOTHNESS) * VIGNETTING_MECHANICAL_SCALE;

    // コサイン4乗則
    const float cosTheta = 1.0f / sqrt(r2 * VIGNETTING_NATURAL_COS_FACTOR + 1.0f);

    vignettingFactor += (1.0f - pow(cosTheta, VIGNETTING_NATURAL_COS_POWER)) * VIGNETTING_NATURAL_SCALE;
    
    float4 Color = 0;
    Color.a = lerp(0, VIGNETTING_COLOR, saturate(vignettingFactor));
    
    return Color;
}