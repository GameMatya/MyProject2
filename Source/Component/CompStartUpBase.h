#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompStartUpBase :public Component
{
public:
  CompStartUpBase(bool* flag, float speed) :displayFlag(flag), startUpSpeed(speed) {}

  float GetStartUpRate() { return startUpRate; }

protected:
  std::weak_ptr<CompSprite2D> sprite;
  bool* displayFlag;

  float startUpRate = 0.0f;
  float startUpSpeed = 1.0f;

  float normalAlpha = 1.0f;

};
