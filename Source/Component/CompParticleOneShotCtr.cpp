#include "CompParticleOneShotCtr.h"

void CompParticleOneShotCtr::Start()
{
  emitterComp = gameObject.lock()->GetComponent<CompParticleEmitter>();
}

void CompParticleOneShotCtr::Update(const float& elapsedTime)
{
  if (*flagPtr == true) {
    emitterComp.lock()->Play();
    *flagPtr = false;
  }
}
