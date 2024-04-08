#include "DepthFunctions.hlsli"
#include "PostProcessDefault.hlsli"

Texture2D sceneTexture : register(t0);
Texture2D depthTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    // 深度値を取得
    float depth = depthTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord).r;
    // 深度値を線形に整形する
    depth = LinearDepth(depth);
    
    // fogColor.aで霧を掛ける距離を調整
    float4 outColor = color;
    outColor.rgb = lerp(outColor.rgb, distanceFog.fogColor.rgb, saturate(depth - distanceFog.fogColor.a));
    outColor.rgb = lerp(outColor.rgb, color.rgb, step(1.0,depth + 0.000001 ));
    
    return outColor;
}
