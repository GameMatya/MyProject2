#pragma once

#include "Component.h"

class CompCameraFree :public Component
{
public:
  CompCameraFree(){}

  void Update(const float& elapsedTime)override;

  void DrawImGui()override;

  const char* GetName() const override { return "Camera Free"; };

#pragma region セッター
  void SetTargetFocus(DirectX::XMFLOAT3 targetFocus) { this->targetFocus = targetFocus; }
  void SetTargetEye(DirectX::XMFLOAT3 targetEye) { this->targetEye = targetEye; }
  void SetCurrentFocus(DirectX::XMFLOAT3 currentFocus) { this->currentFocus = currentFocus; }
  void SetCurrentEye(DirectX::XMFLOAT3 currentEye) { this->currentEye = currentEye; }
  void SetAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; }
  void SetDistance(float distance) { this->distance = distance; }

#pragma endregion

private:
  // 目標のパラメーター
  DirectX::XMFLOAT3  targetFocus = { 0,0,0 };
  DirectX::XMFLOAT3  targetEye = { 0,0,0 };
  // 現在のパラメーター
  DirectX::XMFLOAT3  currentFocus = { 0,0,0 };
  DirectX::XMFLOAT3  currentEye = { 0,0,0 };

  DirectX::XMFLOAT3  angle = { 0,0,0 };
  float              distance = 10;

};
