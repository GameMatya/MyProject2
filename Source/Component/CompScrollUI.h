#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "Math/OriginMath.h"

#include <memory>

class CompScrollUI :public Component
{
public:
  CompScrollUI(const float* scrollPtr, bool isLoop, float speed = 1.0f, DirectX::XMFLOAT2 direction = DirectX::XMFLOAT2(1.0f, 0.0f))
    :scrollValue(scrollPtr),isLoop(isLoop), scrollSpeed(speed), scrollDirection(direction) {};

  // ���O�擾
  const char* GetName() const override { return "ScrollMeter"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  // GUI�`��
  virtual void DrawImGui() override;

  void SetScrollDirection(const DirectX::XMFLOAT2& direction) { scrollDirection = direction; }

private:
  // �X�N���[���̊
  const float* scrollValue = nullptr;
  const bool isLoop = false;

  // �����ʒu
  DirectX::XMFLOAT2 initValue = { 0.0f,0.0f };
  float  scrollSpeed = 1.0f;
  DirectX::XMFLOAT2 scrollDirection = { 0.0f,0.0f };
  std::weak_ptr<CompSprite2D> scrollSprite;

};
