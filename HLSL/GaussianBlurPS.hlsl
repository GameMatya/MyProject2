#include "GaussianBlur.hlsli"

Texture2D sceneTexture : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 Color = 0;
    
    for (int i = 0; i < 2; ++i)
    {
        // ��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
        Color += weights[i].x * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4].xy);
        Color += weights[i].y * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 1].xy);
        Color += weights[i].z * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 2].xy);
        Color += weights[i].w * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 3].xy);
        
        // ��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
        Color += weights[i].x * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4].zw);
        Color += weights[i].y * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 1].zw);
        Color += weights[i].z * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 2].zw);
        Color += weights[i].w * sceneTexture.Sample(samplerStates[BORDER_LINEAR], pin.texcoord[i * 4 + 3].zw);
    }
    
    return float4(Color.xyz, 1.0f);
}