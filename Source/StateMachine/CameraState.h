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
  // �e���̃J�����̈ړ��ʂ�Ԃ�
  inline DirectX::XMFLOAT3 AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed = 1.0f);

  // �R���g���[���[�̓��͂����]�s����쐬
  inline DirectX::XMMATRIX CreateRotation(const float& elapsedTime);

  // �J�����I�u�W�F�N�g�̑O�����x�N�g����Angle�ɕϊ�
  void SetAngleFromForward();

private:
  WeakObject ownerObject;

  DirectX::XMFLOAT3  angle = { 0.418f,-1.285f,0.0f };

  // ��]���x
  const float       ROLL_SPEED = DirectX::XMConvertToRadians(90);

  // �p�x����
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
  // �e���̃J�����̈ړ��ʂ�Ԃ�
  inline DirectX::XMFLOAT3 AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed = 1.0f);

private:
  WeakObject ownerObject;
  WeakObject targetObject;

  // ���b�N�I���̊ɂ� ( ������ 0 �` 5 �ɂ� : ���ς��g���� )
  float       RockOnPower = 2.0f;
  const float ROCKON_POWER_Y = 0.5f;

};