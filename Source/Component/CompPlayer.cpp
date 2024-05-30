#include "CompPlayer.h"
#include "Camera/Camera.h"

#include "System/Input.h"
#include "Scenes/SceneGame.h"
#include "SceneSystem/Scene.h"
#include "StateMachine/PlayerState.h"

#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"

#include "imgui.h"

void CompPlayer::Start()
{
  CompCharacter::Start();

  // �}�l�[�W���[�Ɏ��g��o�^
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);

  actionStateMachine.AddState(std::make_shared<PlayerIdle>(this));
  actionStateMachine.AddState(std::make_shared<PlayerMove>(this));
  actionStateMachine.AddState(std::make_shared<PlayerDodge>(this));

  // �����X�e�[�g��ݒ�
  actionStateMachine.ChangeState(ACTION_STATE::IDLE);
}

void CompPlayer::Update(const float& elapsedTime)
{
  // �ړ����͂̍X�V
  UpdateInputVector();

  // �e�s���̍X�V
  actionStateMachine.Update(elapsedTime);

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

void CompPlayer::Move(DirectX::XMFLOAT3 direction)
{
  float moveSpeed = 0.0f;

  switch (condition)
  {
  case CompPlayer::CONDITION::NORMAL:
    moveSpeed = MOVE_SPEED_NO_WEAPON;
    break;
  case CompPlayer::CONDITION::USE_ITEM:
    moveSpeed = MOVE_SPEED_USE_ITEM;
    break;
  case CompPlayer::CONDITION::WEAPON_AXE:
    moveSpeed = MOVE_SPEED_AXE;
    break;
  case CompPlayer::CONDITION::WEAPON_SWORD:
    moveSpeed = MOVE_SPEED_SWORD;
    break;
  default:
    assert(!"condition error");
    break;
  }

  // ���N���X��Move�֐����Ăяo��
  CompCharacter::Move(direction, moveSpeed);
}

bool CompPlayer::InputDodge()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_A);
}

void CompPlayer::UpdateInputVector()
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

  // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
  Camera& camera = Camera::Instance();
  const DirectX::XMFLOAT3& rightVec = camera.GetRight();
  const DirectX::XMFLOAT3& forwardVec = camera.GetForward();

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
