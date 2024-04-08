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

        float theta = lerp(-PI, PI, random(id + _time.x));
        float p = lerp(0.0, 1.0, random(theta + id * _time.x));
        float phi = asin((2.0f * p) - 1.0f);
    
        particle.position.x += cos(phi) * cos(theta) * random(theta * phi + id) * spawnRandScale;
        particle.position.y += sin(phi) * random(phi * id + p) * spawnRandScale;
        particle.position.z += cos(phi) * sin(theta) * random(p * _time.x + id) * spawnRandScale;
    }
    
    // 移動ベクトル
    {   
        particle.moveVector = moveVec;
        particle.moveVector.x += ((random(particle.position.x * _time.x + id) * 2) - 1) * vecRandScale;
        particle.moveVector.y += ((random(particle.position.y * _time.x + id) * 2) - 1) * vecRandScale;
        particle.moveVector.z += ((random(particle.position.z * _time.x + id) * 2) - 1) * vecRandScale;
        
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
