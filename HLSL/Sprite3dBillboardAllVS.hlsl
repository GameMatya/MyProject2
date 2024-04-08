#include "Sprite3d.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    
    // ビルボード処理
    {
        // モデルの原点をビュー変換
        float3 viewPos = mul(float4(0, 0, 0, 1), _view).xyz;
                    
        // スケールと回転（平行移動なし）だけModel変換して、View変換はスキップ
        float3 scaleRotate = mul((float3x3) worldMatrix, position);
                    
        // scaleRotatePosを右手系に変換して、viewPosに加算
        // zのみ反転
        viewPos += float3(scaleRotate.xy, -scaleRotate.z);
                    
        vout.position = mul(float4(viewPos, 1), _projection);
    }
    
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = true;

    return vout;
}
