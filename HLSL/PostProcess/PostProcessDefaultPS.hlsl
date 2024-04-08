#include "PostProcessDefault.hlsli"

Texture2D texture0 : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return texture0.Sample(samplerStates[BORDER_POINT], pin.texcoord);
}
