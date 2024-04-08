#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(THREAD_NUM_X, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    particleBuffer[DTid.x].isAlive = false;
    particleBuffer[DTid.x].lifeTimer = -1.0;
    particleBuffer[DTid.x].size = 0;
    particleBuffer[DTid.x].color = 0;
    deadList.Append(DTid.x); // 未使用リスト(AppendStructuredBuffer)の末尾に追加
}
