
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
    float ASPECT_RATIO;
    float VIGNETTING_MECHANICAL_RADIUS2;
    float VIGNETTING_MECHANICAL_INV_SMOOTHNESS;
    float VIGNETTING_MECHANICAL_SCALE;
    
    float VIGNETTING_NATURAL_COS_FACTOR;
    float VIGNETTING_NATURAL_COS_POWER;
    float VIGNETTING_NATURAL_SCALE;
    float VIGNETTING_COLOR;
}
