#include "Constants.hlsli"

static const uint THREAD_NUM_X = 16;

// ルビコン星の重力
static const float GRAVITY = 0.68f;

struct VS_OUT
{
    uint vertexId : ID;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct ParticleData
{
    float3  position;
    float   size;
    float   speed;
    float3  moveVector;
    float   gravityAccume;
    float4  color;
    float   emissivePower;
    float   lifeTimer;
    bool    isAlive;
};

cbuffer EmitterData : register(b6)
{
    float3 emitPosition; // パーティクルの生成位置
    float gravityModifier; // 重力の影響を受ける

    float lifeTime; // パーティクルの生存時間
    float emissivePower; // 発光の強さ
    float spawnRandScale; // 生成位置のランダムの大きさ
    float dummy; // パディング

    float3 moveVec; // 進行方向
    float dummy2; // パディング
    
    float vecRandScale; // 進行方向のランダムの大きさ
    float startSize; // 生成時の大きさ
    float endSize; // 消滅時の大きさ
    float dummy3; // パディング

    float particleSpeed; // パーティクルの速度
    float speedRandScale; // 速度のランダムの大きさ
    float startSpeedRate; // 生成時の速さの倍率
    float endSpeedRate; // 消滅時の速さの倍率
    
    float4 startColor; // 生成時の色
    float4 endColor; // 消滅時の色
};