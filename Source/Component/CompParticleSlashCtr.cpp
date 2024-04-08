#include "CompParticleSlashCtr.h"

CompParticleSlashCtr::CompParticleSlashCtr(GameObject* owner)
{
  ownerComp = owner->GetComponent<CompArmoredCore>();
}

void CompParticleSlashCtr::Start()
{
  emitterComp = GetGameObject()->GetComponent<CompParticleEmitter>();
}

void CompParticleSlashCtr::Update(const float& elapsedTime)
{
  if (ownerComp.expired() == true)return;
  emitterComp.lock()->SetIsLoop(ownerComp.lock()->GetIsSlashEffect());
}
