#include "Particle.hlsli"
#include "Rand.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    ParticleData particle = particleBuffer[DTid.x];
    
    // �������s���Ă�����X�V���Ȃ�
    if (particle.isAlive)
    {
        float deltaTime = _time.y;
    
        // �����X�V
        particle.lifeTimer -= deltaTime;
    
        float lerpRate = 1 - (particle.lifeTimer / lifeTime);
    
        // �ʒu�X�V
        particle.position += particle.moveVector *
        particle.speed * lerp(startSpeedRate, endSpeedRate, lerpRate) * deltaTime;
    
        // �d��
        particle.gravityAccume += GRAVITY * deltaTime * gravityModifier;
        particle.position.y -= particle.gravityAccume;
    
        // �傫���X�V
        particle.size = lerp(startSize, endSize, lerpRate);
    
        // �F �X�V
        particle.color = lerp(startColor, endColor, lerpRate);
    
        // �������s�����疢�g�p���X�g�֖߂�
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
    
        // �o�b�t�@���������������
        particleBuffer[DTid.x] = particle;
    }
}
