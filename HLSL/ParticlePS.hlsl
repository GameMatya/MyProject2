#include "Particle.hlsli"

// pow�֐��ɕ��̒l������\����񍐂���x���𖳌���
#pragma warning (disable : 3571)

Texture2D particleTex : register(t0);

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = particleTex.Sample(samplerStates[WRAP_LINEAR], pin.texcoord);
    // ���j�A��Ԃɕϊ�
    color.rgb = pow(color.rgb, GAMMA);

    color *= pin.color; 
    
    return color;
}
