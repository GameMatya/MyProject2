#include "Particle.hlsli"
#include "Rand.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    ParticleData particle = particleBuffer[DTid.x];
    
    // 寿命が尽きていたら更新しない
    if (particle.isAlive)
    {
        float deltaTime = _time.y;
    
        // 寿命更新
        particle.lifeTimer -= deltaTime;
    
        float lerpRate = 1 - (particle.lifeTimer / lifeTime);
    
        // 位置更新
        particle.position += particle.moveVector *
        particle.speed * lerp(startSpeedRate, endSpeedRate, lerpRate) * deltaTime;
    
        // 重力
        particle.gravityAccume += GRAVITY * deltaTime * gravityModifier;
        particle.position.y -= particle.gravityAccume;
    
        // 大きさ更新
        particle.size = lerp(startSize, endSize, lerpRate);
    
        // 色 更新
        particle.color = lerp(startColor, endColor, lerpRate);
    
        // 寿命が尽きたら未使用リストへ戻す
        if (particle.lifeTimer <= 0.0)
        {
            particle.isAlive = false;
            particle.lifeTimer = 0.0;
            particle.position = 0;
            particle.size = 0;
            particle.speed = 0;
            particle.moveVector = 0;
            particle.gravityAccume = 0;
            particle.color = 0;
            particle.emissivePower = 0;
        
            deadList.Append(DTid.x);
        }
    
        // バッファから情報を書き込み
        particleBuffer[DTid.x] = particle;
    }
}
