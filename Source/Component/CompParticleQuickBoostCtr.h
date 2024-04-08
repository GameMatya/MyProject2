#pragma once

#include "Component.h"
#include "Math/OriginMath.h"
#include "CompArmoredCore.h"
#include "CompParticleEmitter.h"

#include <memory>

class CompParticleQuickBoostCtr :public Component
{
public:
  CompParticleQuickBoostCtr(GameObject* owner);

  void Start()override;

  void Update(const float& elapsedTime);

  // 以下ゲッター・セッター
  const char* GetName()const override { return "ParticleArmoredCoreCtr"; };

  void DrawImGui()override;

  void SetAcceptRange(const float& range) { acceptRange = range; }

private:
  std::weak_ptr<CompArmoredCore> ownerComp;
  std::weak_ptr<CompParticleEmitter> emitterComp;

  bool  isEnterQB = false;
  float acceptRange = 0.0f;

};


