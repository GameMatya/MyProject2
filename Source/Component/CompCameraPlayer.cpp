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
  UpdateCameraSlope();

  // �X�e�[�g�}�V���̍X�V
  cameraSM.Update(elapsedTime);

  // UI�̍X�V
  UpdatePlayerUI();
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

void CompCameraPlayer::UpdateCameraSlope()
{
  CompPlayer::MAIN_STATE currentState = player.lock()->GetMainSM().GetCurrentState();

  if (player.lock()->GetBoostMode() == true)
  {
    if (currentState == CompPlayer::MAIN_STATE::MOVE) {

      DirectX::XMFLOAT3 velocity = player.lock()->GetVelocity();
      velocity.y = 0.0f;
      float velocityRate = Mathf::Dot(velocity, velocity);
      velocityRate = velocityRate / (player.lock()->GetMaxMoveSpeed() * player.lock()->GetMaxMoveSpeed());
      velocityRate = min(velocityRate, 1.0f);

      slopeApplyRate = std::lerp(slopeApplyRate, SLOPE_RATE_MOVE * velocityRate, 0.1f);
      return;
    }
    if (currentState == CompPlayer::MAIN_STATE::QUICK_BOOST) {
      slopeApplyRate = std::lerp(slopeApplyRate, SLOPE_RATE_BOOST, 0.1f);
      return;
    }
  }
  slopeApplyRate = std::lerp(slopeApplyRate, 0.0f, 0.1f);
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

void CompCameraPlayer::UpdatePlayerUI()
{
  // �U���Ώۂ�����ꍇ�AUI�̈ʒu�����b�N�I���Ώۂ̃X�N���[�����W�ɂ���
  if (player.lock()->GetAttackTarget().expired() == false) {
    Camera& camera = Camera::Instance();

    GameObject* enemy = player.lock()->GetAttackTarget().lock().get();
    CompCharacter* compChara = enemy->GetComponent<CompCharacter>().get();

    // �X�N���[�����W�n�ł̍��W
    DirectX::XMFLOAT3 position = enemy->transform.position;
    position.y += compChara->GetWaistHeight();
    DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
      DirectX::XMLoadFloat3(&position),
      0.0f,	          //	�r���[�|�[�g����X���W
      0.0f,	          //	�r���[�|�[�g����Y���W
      SCREEN_WIDTH,	  //	�r���[�|�[�g��
      SCREEN_HEIGHT,	//	�r���[�|�[�g����
      0.0f,	// �[�x�l�̍ŏ��l
      1.0f,	// �[�x�l�̍ő�l
      DirectX::XMLoadFloat4x4(&camera.GetProjection()),	//	�v���W�F�N�V�����s��
      DirectX::XMLoadFloat4x4(&camera.GetView()),	//	�r���[�s��
      DirectX::XMMatrixIdentity()
    );
    DirectX::XMStoreFloat2(&player.lock()->uiParams.screenPos, ScreenPosition);
  }
  // �U���Ώۂ����Ȃ��ꍇ�A�X�N���[���̒��S��UI��z�u
  else {
    player.lock()->uiParams.screenPos = DirectX::XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f;
  }
}
