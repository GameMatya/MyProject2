#include "DepthFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D blurTexture : register(t0);
Texture2D depthTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float depth = depthTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord).r;
    // 深度値を線形に整形する
    depth = LinearDepth(depth);
    
    // 深度値がnear値より小さかったらピクセルキル
    clip(depth - _cameraScope.x);
    
    float4 Color = blurTexture.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    
    // 深度値から不透明度を計算する
    Color.a = min((depth - _cameraScope.x) / _cameraScope.y, 1);
    
    return Color;
}