#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompThrusterBaseUI :public Component
{
public:
  CompThrusterBaseUI(float* paramPtr) :paramValue(paramPtr) {};

  // 名前取得
  const char* GetName() const override { return "ThrusterBaseUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override;

private:
  // スラスター残量
  float* paramValue = nullptr;
  float  timer = 0.0f;
  float  timerSpeed = 7.5f;
  std::weak_ptr<CompSprite2D> sprite;

};
