#include "easing.h"
#include <DirectXMath.h>
//------------------------------------------------------------------------------
// 
//  イージング関数
// 
//------------------------------------------------------------------------------

#define PI DirectX::XM_PI
// Sine  //t : time  b : totaltime c : max d : min
float Sine::easeIn(float t, float b, float c, float d) { 
    return -c * cosf(t / d * (DirectX::XM_PI / 2)) + c + b;
}

float Sine::easeOut(float t, float b, float c, float d) {
    return c * sinf(t / d * (DirectX::XM_PI / 2)) + b;
}

float Sine::easeInOut(float t, float b, float c, float d) {
    return -c / 2 * (cosf(DirectX::XM_PI*t / d) - 1) + b;
}

// Quad  //t : time  b : totaltime c : max d : min
float Quad::easeIn(float t, float b, float c, float d) { 
    return c * (t /= d) * t + b;
}
float Quad::easeOut(float t, float b, float c, float d) {
    return -c * (t /= d) * (t - 2) + b;
}

float Quad::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return ((c / 2) * (t * t)) + b;
    return -c / 2 * (((t - 2) * (--t)) - 1) + b;
}

// Cubic  //t : time  b : min  c : max d :totaltime
float Cubic::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t + b;
}
float Cubic::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t + 1) + b;
}

float Cubic::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t + 2) + b;
}

// Quart  //t : time  b : totaltime c : max d : min
float Quart::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t + b;
}
float Quart::easeOut(float t, float b, float c, float d) {
    return -c * ((t = t / d - 1) * t * t * t - 1) + b;
}

float Quart::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t + b;
    return -c / 2 * ((t -= 2) * t * t * t - 2) + b;
}

// Quint  //t : time  b : totaltime c : max d : min
float Quint::easeIn(float t, float b, float c, float d) {
    return c * (t /= d) * t * t * t * t + b;
}
float Quint::easeOut(float t, float b, float c, float d) {
    return c * ((t = t / d - 1) * t * t * t * t + 1) + b;
}

float Quint::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return c / 2 * t * t * t * t * t + b;
    return c / 2 * ((t -= 2) * t * t * t * t + 2) + b;
}

// Expo  //t : time  b : totaltime c : max d : min
float Expo::easeIn(float t, float b, float c, float d) {
    return (t == 0) ? b : c * powf(2, 10 * (t / d - 1)) + b;
}
float Expo::easeOut(float t, float b, float c, float d) {
    return (t == d) ? b + c : c * (-powf(2, -10 * t / d) + 1) + b;
}

float Expo::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;
    if (t == d) return b + c;
    if ((t /= d / 2) < 1) return c / 2 * powf(2, 10 * (t - 1)) + b;
    return c / 2 * (-powf(2, -10 * --t) + 2) + b;
}

// Circ  //t : time  b : totaltime c : max d : min
float Circ::easeIn(float t, float b, float c, float d) {
    return -c * (sqrtf(1 - (t /= d) * t) - 1) + b;
}
float Circ::easeOut(float t, float b, float c, float d) {
    return c * sqrtf(1 - (t = t / d - 1) * t) + b;
}

float Circ::easeInOut(float t, float b, float c, float d) {
    if ((t /= d / 2) < 1) return -c / 2 * (sqrtf(1 - t * t) - 1) + b;
    return c / 2 * (sqrtf(1 - t * (t -= 2)) + 1) + b;
}

// Back  //t : time  b : totaltime c : back d : min
float Back::easeIn(float t, float b, float c, float d) {
    float s = 1.70158f;
    float postFix = t /= d;
    return c * (postFix)*t * ((s + 1) * t - s) + b;
}
float Back::easeOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    return c * ((t = t / d - 1) * t * ((s + 1) * t + s) + 1) + b;
}

float Back::easeInOut(float t, float b, float c, float d) {
    float s = 1.70158f;
    if ((t /= d / 2) < 1) return c / 2 * (t * t * (((s *= (1.525f)) + 1) * t - s)) + b;
    float postFix = t -= 2;
    return c / 2 * ((postFix)*t * (((s *= (1.525f)) + 1) * t + s) + 2) + b;
}


// Elastic
float Elastic::easeIn(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    float postFix = a * powf(2, 10 * (t -= 1));
    return -(postFix * sinf((t * d - s) * (2 * PI) / p)) + b;
}

float Elastic::easeOut(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d) == 1) return b + c;
    float p = d * .3f;
    float a = c;
    float s = p / 4;
    return (a * powf(5, -10 * t) * sinf((t * d - s) * (2 * PI) / p) + c + b);
}

float Elastic::easeInOut(float t, float b, float c, float d) {
    if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
    float p = d * (.3f * 1.5f);
    float a = c;
    float s = p / 4;

    if (t < 1) {
        float postFix = a * powf(2, 10 * (t -= 1)); 
        return -.5f * (postFix * sinf((t * d - s) * (2 * PI) / p)) + b;
    }
    float postFix = a * powf(2, -10 * (t -= 1)); 
    return postFix * sinf((t * d - s) * (2 * PI) / p) * .5f + c + b;
}

// Bounce  //t : time  b : totaltime c : max d : min
float Bounce::easeIn(float t, float b, float c, float d) {
    return c - easeOut(d - t, 0, c, d) + b;
}
float Bounce::easeOut(float t, float b, float c, float d) {
    if ((t /= d) < (1 / 2.75f)) {
        return c * (7.5625f * t * t) + b;
    }
    else if (t < (2 / 2.75f)) {
        float postFix = t -= (1.5f / 2.75f);
        return c * (7.5625f * (postFix)*t + .75f) + b;
    }
    else if (t < (2.5 / 2.75)) {
        float postFix = t -= (2.25f / 2.75f);
        return c * (7.5625f * (postFix)*t + .9375f) + b;
    }
    else {
        float postFix = t -= (2.625f / 2.75f);
        return c * (7.5625f * (postFix)*t + .984375f) + b;
    }
}

float Bounce::easeInOut(float t, float b, float c, float d) {
    if (t < d / 2) return easeIn(t * 2, 0, c, d) * .5f + b;
    else return easeOut(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
}