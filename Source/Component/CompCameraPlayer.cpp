#include "CompCameraPlayer.h"
#include "System/framework.h"
#include "Camera/Camera.h"
#include "StateMachine/CameraState.h"

void CompCameraPlayer::Start()
{
  // �X�e�[�g�ǉ�
  cameraSM.AddState(std::make_shared<CameraStateFollow>(this));
  cameraSM.AddState(std::make_shared<CameraStateRockOn>(this));

  // �����X�e�[�g��ݒ�
  cameraSM.ChangeState(CAMERA_STATE::FOLLOW);

  oldPlayerPosition = player.lock()->GetGameObject()->transform.position;
}

void CompCameraPlayer::Update(const float& elapsedTime)
{
  if (player.expired() == true)return;

  // �I�u�W�F�N�g�����炩�ɒǂ�������
  oldPlayerPosition = Mathf::Lerp(oldPlayerPosition, GetPlayerPos(), CompCameraPlayer::CHASE_SPEED * elapsedTime);
  // �O�̃t���[���̈ʒu��ۑ�
  oldPosition = gameObject.lock()->transform.position;

  UpdateDistance();

  // �X�e�[�g�}�V���̍X�V
  cameraSM.Update(elapsedTime);
}

void CompCameraPlayer::UpdateDistance()
{
  CompPlayer::MAIN_STATE currentState = player.lock()->GetMainSM().GetCurrentState();

  float nextDistance = DISTANCE;

  // �X�e�[�g���؂�ւ������ 
  {
    // �ߐڍU�����J�n������
    if (currentState == CompPlayer::MAIN_STATE::SLASH_APPROACH ||
      currentState == CompPlayer::MAIN_STATE::SECOND_APPROACH) {

      nextDistance = DISTANCE_SLASH;
    }
  }

  distance = std::lerp(distance, nextDistance, 0.3f);
}

DirectX::XMFLOAT3 CompCameraPlayer::CalcCameraUp()
{
  if (slopeApplyRate > 0.0f) {
    DirectX::XMFLOAT3 move = gameObject.lock()->transform.position - oldPosition;
    move.y = 0;
    
    // �J�������ړ����Ă���
    if (Mathf::Dot(move, move) >= 0.0001f) {
      move = Mathf::Normalize(move);

      DirectX::XMFLOAT3 newUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
      newUp += move * slopeApplyRate;
      newUp = Mathf::Normalize(newUp);

      cameraUp = Mathf::Lerp(cameraUp, newUp, 0.5f);
    }

    return cameraUp;
  }

  return DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
}
