#include "CompPlayer.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "Scenes/SceneGame.h"

#include "SceneSystem/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"

#include "imgui.h"
#include <string>

void CompPlayer::Start()
{
  CompCharacter::Start();

  // �}�l�[�W���[�Ɏ��g��o�^
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);
}

void CompPlayer::Update(const float& elapsedTime)
{
  // �ړ����͂̍X�V
  UpdateInputMove();

  // ���G���ԍX�V
  UpdateInvincibleTimer(elapsedTime);

  // ���͏����X�V
  UpdateVelocity(elapsedTime);
}

void CompPlayer::DrawImGui()
{
  CompCharacter::DrawImGui();
  ImGui::Separator();

  {
    ModelAnimator& animator = GetModel()->animator;
    float rate = -1.0f;

    if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM))
      rate = animator.GetAnimationRate(ModelAnimator::ANIM_AREA::BOTTOM);

    ImGui::InputFloat("AnimeRate", &rate);
  }

  ImGui::InputFloat("Slope", &slopeRate);
  ImGui::InputFloat("Accele", &accelerate);
}

void CompPlayer::UpdateInputMove()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();
  float ax = gamePad.GetAxisLX();
  float ay = gamePad.GetAxisLY();

  // ���͂�������Ή������Ȃ�
  if (ax == 0.0f && ay == 0.0f) {
    inputVector = { 0.0f,0.0f,0.0f };
    return;
  }

#if(0)
  DirectX::XMFLOAT3 rightVec = {};
  DirectX::XMFLOAT3 forwardVec = {};

  // ���b�N�I�����͓G�Ɍ������x�N�g�������ɂ���
  if (isRockOn == true) {
    forwardVec = GetAttackTargetVec();

    // �O�ςŉE�x�N�g�������߂�
    DirectX::XMFLOAT3 up = { 0,1,0 };
    rightVec = Mathf::Cross(up, forwardVec);
  }
  // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
  else {
    Camera& camera = Camera::Instance();
    rightVec = camera.GetRight();
    forwardVec = camera.GetForward();
}
#else

  // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
  Camera& camera = Camera::Instance();
  const DirectX::XMFLOAT3& rightVec = camera.GetRight();
  const DirectX::XMFLOAT3& forwardVec = camera.GetForward();

#endif

  // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
  // �E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
  DirectX::XMFLOAT2 rightXZ = { rightVec.x,rightVec.z };
  rightXZ = Mathf::Normalize(rightXZ);

  // �O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
  DirectX::XMFLOAT2 forwardXZ = { forwardVec.x,forwardVec.z };
  forwardXZ = Mathf::Normalize(forwardXZ);

  // �X�e�B�b�N�̐������͒l���E�����ɔ��f���A
  // �X�e�B�b�N�̐������͒l��O�����ɔ��f���A
  // �i�s�x�N�g�����v�Z����
  inputVector.x = forwardXZ.x * ay + rightXZ.x * ax;
  inputVector.y = 0;
  inputVector.z = forwardXZ.y * ay + rightXZ.y * ax;
}
