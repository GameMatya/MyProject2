#include "Particle.hlsli"

StructuredBuffer<ParticleData> particleBuffer : register(t0);

static const float3 BILL_BOARD[] =
{
    float3(-0.5f, +0.5f, 0.0f), // ����
	float3(+0.5f, +0.5f, 0.0f), // �E��
    float3(-0.5f, -0.5f, 0.0f), // ����
	float3(+0.5f, -0.5f, 0.0f), // �E��
};
static const float2 TEXCOORD[] =
{
    float2(0.0f, 0.0f), // ����
	float2(1.0f, 0.0f), // �E��
    float2(0.0f, 1.0f), // ����
	float2(1.0f, 1.0f), // �E��
};

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
    ParticleData p = particleBuffer[input[0].vertexId];
    
    float4 viewPos = mul(float4(p.position, 1.0), _view);

    GS_OUT element;

    // ���_�ǉ�
	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        float3 cornerPos = BILL_BOARD[i] * p.size;

        element.position = mul(float4(viewPos.xyz + cornerPos, 1), _projection);
        element.color = p.color;
        element.color.rgb *= 1 + p.emissivePower;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }
    output.RestartStrip();
}

