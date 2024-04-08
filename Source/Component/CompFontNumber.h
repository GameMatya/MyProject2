#pragma once

#include "Component.h"
#include "CompFont.h"
#include "Math/OriginMath.h"

class CompFontNumber :public Component
{
public:
  // 表示する数字の元になるFloat変数のアドレス、小数の有効桁数
  CompFontNumber(const float* value, int fraction, int field = -1)
    :valuePtr(value), fractionCount(fraction), fieldWidth(field) {};

  // 名前取得
  virtual const char* GetName() const { return "CompFontNumber"; };

  void Start()override;

  void Update(const float& elapsedTime)override;

private:
  const float* valuePtr = nullptr;
  std::weak_ptr<CompFont> font;

  int fractionCount = 0;
  int fieldWidth = -1;

};
