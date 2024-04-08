#include "PostProcessDefault.hlsli"

Texture2D blurTexture0 : register(t0);
Texture2D blurTexture1 : register(t1);
Texture2D blurTexture2 : register(t2);
Texture2D blurTexture3 : register(t3);
Texture2D sceneTexture : register(t4);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[BORDER_POINT], pin.texcoord);
    
    // 小さいバッファほど重みを大きくする(川瀬式ブルーム)
    color += blurTexture0.Sample(samplerStates[BORDER_ANISOTROPIC], pin.texcoord) * 0.15;
    color += blurTexture1.Sample(samplerStates[BORDER_ANISOTROPIC], pin.texcoord) * 0.25;
    color += blurTexture2.Sample(samplerStates[BORDER_ANISOTROPIC], pin.texcoord) * 0.3;
    color += blurTexture3.Sample(samplerStates[BORDER_ANISOTROPIC], pin.texcoord) * 0.3;
    
    color.a = 1;
    
    return color;
}