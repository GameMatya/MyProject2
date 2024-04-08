#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompBooleanSprite :public Component
{
public:
  CompBooleanSprite(const bool* booleanPtr, float speed = 1.0f)
    :displayFlag(booleanPtr), DISPLAY_SPEED(speed) {};

  // 名前取得
  const char* GetName() const override { return "BooleanUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  bool GetIsDisplay() { return (displayValue == 1.0f); }

  float GetDisplayValue() { return displayValue; }

private:
  // スクロールの基準
  const bool* displayFlag = nullptr;
  std::weak_ptr<CompSprite2D> sprite;

  // 初期位置
  const float DISPLAY_SPEED;
  float displayValue = 0.0f;

};
