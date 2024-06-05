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

  accelerate = 3;

  actionStateMachine.AddState(ACTION_STATE::IDLE, std::make_shared<PlayerIdle>(this));
  actionStateMachine.AddState(ACTION_STATE::MOVE, std::make_shared<PlayerMove>(this));
  actionStateMachine.AddState(ACTION_STATE::DODGE, std::make_shared<PlayerDodge>(this));
  actionStateMachine.AddState(ACTION_STATE::MOVE_WEAPON, std::make_shared<PlayerMoveWeapon>(this));

  actionStateMachine.AddState(ACTION_STATE::ATTACK, std::make_shared<PlayerAttack>(this));
  actionStateMachine.AddState(ACTION_STATE::ATTACK_LOOP, std::make_shared<PlayerAttackLoop>(this));

  // �����X�e�[�g��ݒ�
  actionStateMachine.ChangeState(ACTION_STATE::IDLE);

  // �U���h���Ȃǂ�ݒ�
  InitAttackContainer();

  // �}�l�[�W���[�Ɏ��g��o�^
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);
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
  float currentMoveSpeed = GetMoveSpeed();
  float moveSpeed = 0.0f;

  switch (condition)
  {
  case CompPlayer::CONDITION::NORMAL:
    if (dashFlg == true)
      moveSpeed = MOVE_SPEED_DASH;
    else
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
  CompCharacter::Move(direction, std::lerp(currentMoveSpeed, moveSpeed, 0.75f));
}

void CompPlayer::ResetDashFlg()
{
  dashThumbFlg = false;
  dashFlg = false;
}

bool CompPlayer::InputDodge()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_A, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputDash()
{
  GamePad& gamePad = Input::Instance().GetGamePad();

  // R1�{�^����������Ă��� �Ȃ�True
  if (gamePad.GetButtonDown(GamePad::BTN_RIGHT_SHOULDER, EARLY_INPUT_RECEIPT_FRAME) && dashThumbFlg == false) {
    dashFlg = true;
    return dashFlg;
  }
  else if (dashFlg == true) {
    dashFlg = gamePad.GetButton(GamePad::BTN_RIGHT_SHOULDER);
    return dashFlg;
  }

  // R3�{�^���Ń_�b�V�����͂��Ă����ꍇ�ATrue��Ԃ�
  if (dashThumbFlg == true && dashFlg == false) {
    return true;
  }
  else if (gamePad.GetButtonDown(GamePad::BTN_LEFT_THUMB, EARLY_INPUT_RECEIPT_FRAME)) {
    dashThumbFlg = true;
    return true;
  }

  return false;
}

bool CompPlayer::InputWeaponSheathAndDraw()
{
  // �A�C�e���g�p���͉������Ȃ�
  if (condition == CONDITION::USE_ITEM) return false;

  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  // �[�����̏ꍇ
  if (condition == CONDITION::NORMAL) {
    if (gamePad.GetButtonDown(GamePad::BTN_Y, EARLY_INPUT_RECEIPT_FRAME) == false)return false;

    condition = CONDITION::WEAPON_AXE;
  }
  // �������̏ꍇ
  else {
    if (gamePad.GetButtonDown(GamePad::BTN_X, EARLY_INPUT_RECEIPT_FRAME) == false &&
      gamePad.GetButtonDown(GamePad::BTN_RIGHT_SHOULDER, EARLY_INPUT_RECEIPT_FRAME) == false &&
      gamePad.GetButtonDown(GamePad::BTN_LEFT_THUMB, EARLY_INPUT_RECEIPT_FRAME) == false)return false;

    condition = CONDITION::NORMAL;
  }

  // �����E�[���A�j���[�V�����Đ�  
  int animeId = GetModel()->animator.FindAnimationId("WeaponSheatheAndDraw");
  GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::UPPER, animeId, false);

  return true;
}

bool CompPlayer::InputAttack()
{
  // �h����
  auto sequence = GetAttackSequence();

  // Y�h��
  if (InputButtonY()) {
    if (sequence[0] != CompPlayer::ATTACK_ACTION::EMPTY) {
      SetAttackAction(sequence[0]);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return true;
    }
  }

  // B�h��
  if (InputButtonB()) {
    // ���[�v�U���̏ꍇ
    if (sequence[1] == CompPlayer::ATTACK_ACTION::AXE_LOOP) {
      SetAttackAction(CompPlayer::ATTACK_ACTION::EMPTY);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK_LOOP);
      return true;
    }
    // �ʏ�̍U��
    if (sequence[1] != CompPlayer::ATTACK_ACTION::EMPTY) {
      SetAttackAction(sequence[1]);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return true;
    }
  }

  return false;
}

bool CompPlayer::InputButtonY()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_Y, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputButtonB()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_B, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputWeaponTransform()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_RIGHT_TRIGGER, EARLY_INPUT_RECEIPT_FRAME);
}

#define SetAttackSequence(src,dstY,dstB)  attackSequence.emplace(src, std::array<ATTACK_ACTION, 2>({dstY, dstB}));
#define SetActionAnime(action,animeName)  attackAnimeNames.emplace(action, animeName);

void CompPlayer::InitAttackContainer()
{
  // �U���̔h���菇��ݒ�
  {
    SetAttackSequence(ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY);
    SetAttackSequence(ATTACK_ACTION::SHEATH, ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::EMPTY);

#pragma region �����[�h
    // �ҋ@����
    SetAttackSequence(ATTACK_ACTION::AXE_IDLE, ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::AXE_LOOP);

    // Y�h�� 
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::COMBO_AXE_Y2, ATTACK_ACTION::AXE_LOOP);
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y2, ATTACK_ACTION::COMBO_AXE_Y3, ATTACK_ACTION::AXE_LOOP);
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y3, ATTACK_ACTION::EMPTY, ATTACK_ACTION::AXE_LOOP);

    SetAttackSequence(ATTACK_ACTION::AXE_LOOP_END, ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY);

#pragma endregion

#pragma region �����[�h
    // �ҋ@����
    SetAttackSequence(ATTACK_ACTION::SWORD_IDLE, ATTACK_ACTION::COMBO_SWORD_Y1, ATTACK_ACTION::COMBO_SWORD_B1);

    // Y�h�� 
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y1, ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_B1);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B1);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::EMPTY, ATTACK_ACTION::COMBO_SWORD_B1);

    // B�h��
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B1, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B2);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B2, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B3);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B3, ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_B1);

#pragma endregion
  }

  // �A�j���[�V�������e�A�N�V�����ɐݒ�
  {
    // ��
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y1, "AxeAttackComboA_1");
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y2, "AxeAttackComboA_2");
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y3, "AxeAttackComboA_3");

    SetActionAnime(ATTACK_ACTION::AXE_DASH, "AxeAttack_Dash");
    SetActionAnime(ATTACK_ACTION::AXE_LOOP, "AxeAttack_Loop1");
    SetActionAnime(ATTACK_ACTION::AXE_LOOP_END, "AxeAttack_LoopEnd");

    // ��
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y1, "SwordAttackComboA_1");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y2, "SwordAttackComboA_2");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y3, "SwordAttackComboA_3");

    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B1, "SwordAttackComboB_1");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B2, "SwordAttackComboB_2");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B3, "SwordAttackComboB_3");
  }
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
