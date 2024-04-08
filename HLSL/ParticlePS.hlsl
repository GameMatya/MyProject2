#include "Particle.hlsli"

// pow関数に負の値が入る可能性を報告する警告を無効化
#pragma warning (disable : 3571)

Texture2D particleTex : register(t0);

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = particleTex.Sample(samplerStates[WRAP_LINEAR], pin.texcoord);
    // リニア空間に変換
    color.rgb = pow(color.rgb, GAMMA);

    color *= pin.color; 
    
    return color;
}
