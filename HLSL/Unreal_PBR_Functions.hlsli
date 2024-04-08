#include "Constants.hlsli"

// �Q�l�T�C�g
// http://project-asura.com/blog/archives/3124
// https://qiita.com/emadurandal/items/3a8db7bc61438245654d
// https://light11.hatenadiary.com/entry/2020/03/05/220957
// https://rayspace.xyz/CG/contents/Disney_principled_BRDF/

// �U�d��
static const float Dielectric = 0.16f;
static const float Specular = 0.5f;

//------------------------------------------------
//	 �g�U����BRDF
//------------------------------------------------
// ���K�������o�[�g
float3 DiffuseBRDF(float3 albedo)
{
    return albedo / PI;
}


//------------------------------------------------
//	 ���ʔ���BRDF
//------------------------------------------------
// �@�����z�֐� D�� ( GGX/Trowbridge-Reitz )
#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

inline float D_GGX(float NdotH, float3 NxH, float roughness)
{
    float a = NdotH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

// �􉽌����� G�� ( Schlick Ver Disney )
#define G1Func(_dot) (_dot /(_dot * (1 - k)+ k)) 

inline float G_SchlickGGX(float NdotL, float NdotV, float roughness)
{
    float k = (roughness + 1) * (roughness + 1) / 8;
    return G1Func(NdotL) * G1Func(NdotV);
}

// �􉽌����� V�� ( Smith Joint GGX �ߎ��� ) "���ʔ���BRDF�̎����ς��̂Œ���"
inline float V_HeightCorrelatedGGX(float NdotL, float NdotV, float roughness)
{
    float alpha = roughness * roughness;
    float lambdaV = NdotL * (NdotV * (1 - alpha) + alpha);
    float lambdaL = NdotV * (NdotL * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

// �t���l���� F�� ( Schlick )
inline float3 F_Schlick(float3 f0, float EdotH)
{
    return f0 + (1 - f0) * pow(1 - EdotH, 5);
}

// �t���l���� F�� ( Spherical Gaussian : Schlick�̋ߎ� )
inline float3 F_SphericalGaussian(float3 f0, float EdotH)
{
    return f0 + (1 - f0) * exp2((-5.55473 * EdotH - 6.98316) * EdotH);
}

// ���ʔ���BRDF (DGF��Ver)
inline float3 OldSpecularBRDF(float3 harfV, float NdotL, float NdotV, float NdotH, float3 NxH, float rough, float3 f0)
{
    float3 DGF = D_GGX(NdotH, NxH, rough) * G_SchlickGGX(NdotL, NdotV, rough) * F_SphericalGaussian(f0, NdotH);
    return DGF / (4 * NdotL * NdotV);
}

// ���ʔ���BRDF (DVF��Ver)
inline float3 NewSpecularBRDF(float3 harfV, float NdotL, float NdotV, float NdotH, float3 NxH, float rough, float3 f0)
{
    return D_GGX(NdotH, NxH, rough) * V_HeightCorrelatedGGX(NdotL, NdotV, rough) * F_Schlick(f0, NdotH);
}


//------------------------------------------------
//	 PBR_BRDF
//------------------------------------------------
inline float3 OldUnrealBRDF(float3 lightVec, float3 lightColor, float3 view, float3 normal, float3 albedo, float metal, float rough, float3 f0)
{
    float3 harfV = normalize(lightVec + view);
    float NdotL = max(dot(normal, lightVec), 0.0001);
    float NdotV = max(dot(normal, view), 0.0001);
    float NdotH = max(dot(normal, harfV), 0.0001);
    float3 NxH = cross(normal, harfV);
    
    float3 irradiance = lightColor * NdotL;
    float3 Fr = DiffuseBRDF(albedo) * (1 - metal) + OldSpecularBRDF(harfV, NdotL, NdotV, NdotH, NxH, rough, f0);
    
    return Fr * irradiance;
}

inline float3 NewUnrealBRDF(float3 lightVec, float3 lightColor, float3 view, float3 normal, float3 albedo, float metal, float rough, float3 f0)
{
    float3 harfV = normalize(lightVec + view);
    float NdotL = max(dot(normal, lightVec), 0.0001);
    float NdotV = max(dot(normal, view), 0.0001);
    float NdotH = max(dot(normal, harfV), 0.0001);
    float3 NxH = cross(normal, harfV);
    
    float3 irradiance = lightColor * NdotL;
    float3 Fr = DiffuseBRDF(albedo) * (1 - metal) + NewSpecularBRDF(harfV, NdotL, NdotV, NdotH, NxH, rough, f0);
    
    return Fr * irradiance;
}
