#pragma once

#include "CompStartUpBase.h"

class CompStartUpScale :public CompStartUpBase
{
public:
  CompStartUpScale(bool* flag, DirectX::XMFLOAT2 initScale, float speed = 1.0f)
    :CompStartUpBase(flag, speed), initScale(initScale) {};

  // 名前取得
  const char* GetName() const override { return "StartUpReticle"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  // GUI描画
  virtual void DrawImGui() override;

private:
  DirectX::XMFLOAT2 initScale = { 0.0f,0.0f };
  DirectX::XMFLOAT2 normalScale = { 0.0f,0.0f };

};
