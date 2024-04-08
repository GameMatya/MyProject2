#include "sprite.hlsli"
#include "Constants.hlsli"

// éQçl : https://www.shadertoy.com/view/tscSz7

#define DOTS 14
#define DOTOFFSET 0.04
#define SPEED 0.4
#define POWER 3.
#define COLTOP float3(0.3,0.6,1.0)
#define COLBTM float3(1.0,0.4,0.3)

float smin(float a, float b, float k)
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return lerp(b, a, h) - k * h * (1.0 - h);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord - 0.5f;
    uv.y *= -1;

    float d = 9999.;
    float4 color = 1;
    for (int i = 0; i < DOTS; i++)
    {
        float t = pow(frac(_time.x * SPEED + (float(i) * DOTOFFSET)), POWER);
        float s = PI * 2.;
        float2 o = float2(sin(t * s), cos(t * s)) * .2;
        float dist = length(uv + o) - .01;
        if (dist < d)
        {
            color.rgb = lerp(COLTOP, COLBTM, cos(t * s) * .5 + .5);
        }
        
        d = smin(d, dist, 0.04);
    }
    color.a = lerp(color.a, 0, smoothstep(0., 0.005, d));

    // Output to screen
    return color;
}

