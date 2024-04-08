#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompSightUI :public Component
{
public:
  CompSightUI(DirectX::XMFLOAT2* posPtr, bool* drawTermPtr)
    :drawPos(posPtr), drawTerm(drawTermPtr) {};

  // –¼‘OŽæ“¾
  const char* GetName() const override { return "SightUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  DirectX::XMFLOAT2* drawPos = nullptr;
  bool* drawTerm = nullptr;

  std::weak_ptr<CompSprite2D> sightSprite;

};
