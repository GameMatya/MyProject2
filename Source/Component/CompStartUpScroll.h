#pragma once

#include "CompStartUpBase.h"

class CompStartUpScroll :public CompStartUpBase
{
public:
  CompStartUpScroll(bool* flag, DirectX::XMFLOAT2 scrollSpeed, float speed = 1.0f)
    :CompStartUpBase(flag, speed), scrollSpeed(scrollSpeed) {};

  // ���O�擾
  const char* GetName() const override { return "StartUpReticle"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  // GUI�`��
  virtual void DrawImGui() override;

private:
  DirectX::XMFLOAT2 scrollSpeed = { 0.0f,0.0f };

};
