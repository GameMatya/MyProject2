#include "particle.hlsli"
#include "Rand.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main()
{
    // 未使用リストから取り出す
    uint id = particlePool.Consume();

    ParticleData particle = particleBuffer[id];

    // パーティクルの初期位置設定
    {
        particle.position = emitPosition;

        float theta = lerp(-PI, PI, random(float2(asfloat(id) + _time.x, asfloat(id) * _time.x)));
        float p = lerp(0.0, 1.0, random(float2(theta + asfloat(id) * _time.x, theta * asfloat(id))));
        float phi = asin((2.0f * p) - 1.0f);
    
        particle.position.x += cos(phi) * cos(theta) * random(float2(theta * phi + asfloat(id), asfloat(id) * p)) * spawnRandScale;
        particle.position.y += sin(phi) * random(float2(phi * asfloat(id) + p, p * theta + _time.x)) * spawnRandScale;
        particle.position.z += cos(phi) * sin(theta) * random(float2(p * _time.x + asfloat(id), phi * p + theta)) * spawnRandScale;
    }
    
    // 移動ベクトル
    {   
        particle.moveVector = moveVec;
        particle.moveVector.x += ((random(float2(particle.position.x * _time.x + id, cos(particle.position.z))) * 2) - 1) * vecRandScale;
        particle.moveVector.y += ((random(float2(particle.position.y * _time.x + id, sin(particle.position.x))) * 2) - 1) * vecRandScale;
        particle.moveVector.z += ((random(float2(particle.position.z * _time.x + id, cos(particle.position.y))) * 2) - 1) * vecRandScale;
        
        particle.moveVector = normalize(particle.moveVector);
    }
    
    particle.speed = particleSpeed + random(id * _time.x) * speedRandScale;
    
    particle.size = startSize;
    particle.color = startColor;
    particle.gravityAccume = 0.0;
    particle.emissivePower = emissivePower;
    particle.lifeTimer = lifeTime;
    particle.isAlive = true;
    
	// バッファを更新
    particleBuffer[id] = particle;
}
