#include "sprite.hlsli"

// pow�֐��ɕ��̒l������\����񍐂���x���𖳌���
#pragma warning (disable : 3571)

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{    
    float4 color = texture0.Sample(samplerStates[WRAP_POINT], pin.texcoord);
    color.a *= pin.color.a;
    
    // ���������͕`�悵�Ȃ�
    if (!(color.a - EPSILON))
        discard;
    
    color.rgb = pow(color.rgb, max(GAMMA * pin.isLinear, 1)) * pin.color.rgb;
    
    return color;
}