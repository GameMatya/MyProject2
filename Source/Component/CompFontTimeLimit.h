#pragma once

#include "Component.h"
#include "CompFont.h"

#include <memory>

class CompFontTimeLimit :public Component
{
public:
  CompFontTimeLimit(const float* timerPtr, float border = 15.0f)
    :timer(timerPtr), CAUTION_BORDER(border) {};

  // ���O�擾
  const char* GetName() const override { return "TimeLimitUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  const float* timer = nullptr;
  std::weak_ptr<CompFont> font;

  // �ԐF�ɂ������
  const float CAUTION_BORDER;
  const DirectX::XMFLOAT4 CAUTION_COLOR = { 1.5f,0.1f,0.0f,1.0f };
};
