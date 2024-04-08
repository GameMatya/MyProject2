#pragma once

#include "Math/OriginMath.h"

//カメラコントローラー
class CameraController
{
public:
  CameraController() {}
  ~CameraController() {}

  //更新処理
  void Update(const float& elapsedTime);

  void UpdateDebug(const float& elapsedTime);

  //ターゲット位置設定
  void SetTarget(const DirectX::XMFLOAT3& target) { this->targetFocus = target; }

private:
  // 目標のパラメーター
  DirectX::XMFLOAT3  targetFocus = { 0,0,0 };
  DirectX::XMFLOAT3  targetEye = { 0,0,0 };
  // 現在のパラメーター
  DirectX::XMFLOAT3  currentFocus = { 0,0,0 };
  DirectX::XMFLOAT3  currentEye = { 0,0,0 };

  DirectX::XMFLOAT3  angle = { 0,0,0 };
  float              distance = 10;

  float              ROLL_SPEED = DirectX::XMConvertToRadians(90);
  float              range = 10.0f;
  float              MAX_ANGLE_X = DirectX::XMConvertToRadians(45);
  float              MIN_ANGLE_X = DirectX::XMConvertToRadians(-45);

};
