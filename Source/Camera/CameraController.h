#pragma once

#include "Math/OriginMath.h"

//�J�����R���g���[���[
class CameraController
{
public:
  CameraController() {}
  ~CameraController() {}

  //�X�V����
  void Update(const float& elapsedTime);

  void UpdateDebug(const float& elapsedTime);

  //�^�[�Q�b�g�ʒu�ݒ�
  void SetTarget(const DirectX::XMFLOAT3& target) { this->targetFocus = target; }

private:
  // �ڕW�̃p�����[�^�[
  DirectX::XMFLOAT3  targetFocus = { 0,0,0 };
  DirectX::XMFLOAT3  targetEye = { 0,0,0 };
  // ���݂̃p�����[�^�[
  DirectX::XMFLOAT3  currentFocus = { 0,0,0 };
  DirectX::XMFLOAT3  currentEye = { 0,0,0 };

  DirectX::XMFLOAT3  angle = { 0,0,0 };
  float              distance = 10;

  float              ROLL_SPEED = DirectX::XMConvertToRadians(90);
  float              range = 10.0f;
  float              MAX_ANGLE_X = DirectX::XMConvertToRadians(45);
  float              MIN_ANGLE_X = DirectX::XMConvertToRadians(-45);

};
