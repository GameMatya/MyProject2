#include "Sprite3d.hlsli"

VS_OUT main(float3 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
    VS_OUT vout;
    
    // �r���{�[�h����
    {
        // ���f���̌��_���r���[�ϊ�
        float3 viewPos = mul(float4(0, 0, 0, 1), _view).xyz;
                    
        // �X�P�[���Ɖ�]�i���s�ړ��Ȃ��j����Model�ϊ����āAView�ϊ��̓X�L�b�v
        float3 scaleRotate = mul((float3x3) worldMatrix, position);
                    
        // scaleRotatePos���E��n�ɕϊ����āAviewPos�ɉ��Z
        // z�̂ݔ��]
        viewPos += float3(scaleRotate.xy, -scaleRotate.z);
                    
        vout.position = mul(float4(viewPos, 1), _projection);
    }
    
    vout.color = color;
    vout.texcoord = texcoord;
    vout.isLinear = true;

    return vout;
}
