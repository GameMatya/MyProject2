#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompThrusterUI :public Component
{
public:
  CompThrusterUI(float* scrollPtr) :scrollValue(scrollPtr) {};

  // ���O�擾
  const char* GetName() const override { return "ThrusterUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  // �����p��
  float initTexSizeX = 0.0f;
  // �����̊
  float* scrollValue = nullptr;
  std::weak_ptr<CompSprite2D> scrollSprite;

};
