#pragma once

#include "Component.h"
#include "Math/OriginMath.h"
#include "CompArmoredCore.h"
#include "CompParticleEmitter.h"

#include <memory>

class CompParticleSlashCtr :public Component
{
public:
  CompParticleSlashCtr(GameObject* owner);

  void Start()override;

  void Update(const float& elapsedTime);

  // 以下ゲッター・セッター
  const char* GetName()const override { return "ParticleSlashCtr"; };

private:
  std::weak_ptr<CompArmoredCore> ownerComp;
  std::weak_ptr<CompParticleEmitter> emitterComp;

};


