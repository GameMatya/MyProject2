#include "../sprite.hlsli"

Texture2D texture0 : register(t0);

static const float2 SCREEN_SIZE = float2(1920, 1080);
static const float CURVE_RATE = 0.2;

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 offset = 1.0;
    offset.x = abs((pin.position.y / SCREEN_SIZE.y) - 0.5) / 0.5;
    offset.x = 1 - offset.x * offset.x * offset.x;
    offset.x *= ((pin.position.x / SCREEN_SIZE.x) * 2 - 1);
    
    float4 color = texture0.Sample(samplerStates[WRAP_POINT], pin.texcoord + offset * CURVE_RATE);
    color.a *= pin.color.a;
    
    // “§–¾•”•ª‚Í•`‰æ‚µ‚È‚¢
    if (!(color.a - EPSILON))
        discard;
    
    return color;
}