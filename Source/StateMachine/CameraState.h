#pragma once

#include "State.h"
#include "Component/CompCameraPlayer.h"

class CompCameraPlayer;

class CameraStateFollow :public State<CompCameraPlayer>
{
public:
  CameraStateFollow(CompCameraPlayer* owner);
  void Enter() override;
  void Execute(const float& elapsedTime) override;

private:
  // 各軸のカメラの移動量を返す
  inline DirectX::XMFLOAT3 AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed = 1.0f);

  // コントローラーの入力から回転行列を作成
  inline DirectX::XMMATRIX CreateRotation(const float& elapsedTime);

  // カメラオブジェクトの前方向ベクトルをAngleに変換
  void SetAngleFromForward();

private:
  WeakObject ownerObject;

  DirectX::XMFLOAT3  angle = { 0.418f,-1.285f,0.0f };

  // 回転速度
  const float       ROLL_SPEED = DirectX::XMConvertToRadians(90);

  // 角度制限
  const float       MAX_ANGLE_X = DirectX::XMConvertToRadians(45);
  const float       MIN_ANGLE_X = DirectX::XMConvertToRadians(-45);

};

class CameraStateRockOn :public State<CompCameraPlayer>
{
public:
  CameraStateRockOn(CompCameraPlayer* owner);
  void Enter() override;
  void Execute(const float& elapsedTime) override;

private:
  // 各軸のカメラの移動量を返す
  inline DirectX::XMFLOAT3 AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed = 1.0f);

private:
  WeakObject ownerObject;
  WeakObject targetObject;

  // ロックオンの緩さ ( 厳しい 0 ～ 5 緩い : 内積を使う為 )
  float       RockOnPower = 2.0f;
  const float ROCKON_POWER_Y = 0.5f;

};