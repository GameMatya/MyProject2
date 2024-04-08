#include "Sprite3d.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    
    // ビルボード処理
    {
        float3 worldPos = worldMatrix._41_42_43;
        float3 toCamera = _cameraPosition.xyz - worldPos;

        float3 yUp = billboardAxis.xyz;
        float3 up = mul(yUp, (float3x3) worldMatrix);
        float3 right = normalize(cross(toCamera, up)) * length(worldMatrix._11_12_13);
        float3 forward = normalize(cross(up, right)) * length(worldMatrix._31_32_33);

        float4x4 mat =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        mat._11_12_13 = right;
        mat._21_22_23 = up;
        mat._31_32_33 = forward;
        mat._41_42_43 = worldPos;

        float4 vertex = float4(position.xyz, 1);
        vertex = mul(vertex, mat);
        vout.position = mul(vertex, _viewProjection);
    }
    
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = true;

    return vout;
}
