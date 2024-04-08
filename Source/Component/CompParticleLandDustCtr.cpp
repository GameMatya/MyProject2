#include "CompParticleLandDustCtr.h"

CompParticleLandDustCtr::CompParticleLandDustCtr(GameObject* owner)
{
  ownerComp = owner->GetComponent<CompArmoredCore>();
}

void CompParticleLandDustCtr::Start()
{
  emitterComp = GetGameObject()->GetComponent<CompParticleEmitter>();
}

void CompParticleLandDustCtr::Update(const float& elapsedTime)
{
  DirectX::XMFLOAT3 velocity = ownerComp.lock()->GetVelocity();
  velocity.y = 0;

  // 接地状態かつ移動値がある場合、パーティクル生成
  bool isEmit = (ownerComp.lock()->GetIsGround() == true && Mathf::Dot(velocity, velocity) > 0);
  emitterComp.lock()->SetIsLoop(isEmit);

  // 歩行時はパーティクルの生成間隔を設定する
  emitterComp.lock()->SetEmitInterval(0.05f * (float)!ownerComp.lock()->GetBoostMode());
}

