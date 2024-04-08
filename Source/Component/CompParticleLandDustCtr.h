#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Component.h"
#include "Math/OriginMath.h"
#include "Graphics/ShaderStaties.h"
#include "CompArmoredCore.h"
#include "CompParticleEmitter.h"

class CompParticleLandDustCtr :public Component
{
public:
  CompParticleLandDustCtr(GameObject* owner);

  void Start()override;

  void Update(const float& elapsedTime);

  // 以下ゲッター・セッター
  const char* GetName()const override { return "ParticleLandDustCtr"; };

private:
  std::weak_ptr<CompArmoredCore> ownerComp;
  std::weak_ptr<CompParticleEmitter> emitterComp;

};


