#ifndef	__CONSTANTS_HLSLI__
#define	__CONSTANTS_HLSLI__

// ‰~Žü—¦
static const float PI = 3.141592653589;
// ƒKƒ“ƒ}’l
static const float GAMMA = 2.2;
// ŒÀ‚è‚È‚­0‚É‹ß‚¢’l
static const float EPSILON = 0.000001;

 #define WRAP_POINT 0
 #define WRAP_LINEAR 1
 #define WRAP_ANISOTROPIC 2

 #define BORDER_POINT 3
 #define BORDER_LINEAR 4
 #define BORDER_ANISOTROPIC 5

SamplerState samplerStates[6] : register(s0);

cbuffer CommonConstant : register(b5)
{
    row_major float4x4 _view;
    row_major float4x4 _projection;
    row_major float4x4 _viewProjection;
    row_major float4x4 _inverseView;
    row_major float4x4 _inverseProjection;

    float4 _cameraPosition;
    float4 _cameraScope; // x = Near, y = Far, z & w = Padding

    float4 _time; // x = TotalTime, y = ElapsedTime, z & w = Padding
}

#endif // #endif __CONSTANTS_HLSLI__