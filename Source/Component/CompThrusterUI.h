#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompThrusterUI :public Component
{
public:
  CompThrusterUI(float* scrollPtr) :scrollValue(scrollPtr) {};

  // –¼‘Oæ“¾
  const char* GetName() const override { return "ThrusterUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  // ‰Šúp¨
  float initTexSizeX = 0.0f;
  // ‘Œ¸‚ÌŠî€
  float* scrollValue = nullptr;
  std::weak_ptr<CompSprite2D> scrollSprite;

};
