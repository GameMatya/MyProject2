#pragma once

#include "Component.h"
#include "Math/OriginMath.h"
#include "CompArmoredCore.h"
#include "CompParticleEmitter.h"

#include <memory>

class CompParticleOneShotCtr :public Component
{
public:
  CompParticleOneShotCtr(bool* flag) :flagPtr(flag) {};

  void Start()override;

  void Update(const float& elapsedTime);

  // �ȉ��Q�b�^�[�E�Z�b�^�[
  const char* GetName()const override { return "ParticleOneShot"; };

private:
  bool* flagPtr = nullptr;
  std::weak_ptr<CompParticleEmitter> emitterComp;

};
