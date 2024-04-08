#include "DepthFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D blurTexture : register(t0);
Texture2D depthTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float depth = depthTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord).r;
    // �[�x�l����`�ɐ��`����
    depth = LinearDepth(depth);
    
    // �[�x�l��near�l��菬����������s�N�Z���L��
    clip(depth - _cameraScope.x);
    
    float4 Color = blurTexture.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    
    // �[�x�l����s�����x���v�Z����
    Color.a = min((depth - _cameraScope.x) / _cameraScope.y, 1);
    
    return Color;
}