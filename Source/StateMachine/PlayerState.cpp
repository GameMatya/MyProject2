#include "PlayerState.h"

#pragma region �ҋ@���

void PlayerIdle::Enter()
{
  // �ҋ@�A�j���[�V�����Đ�  
  int animeId = -1;
  CompPlayer::CONDITION condition = owner->GetCondition();

  if (condition == CompPlayer::CONDITION::NORMAL) {
    animeId = owner->GetModel()->animator.FindAnimationId("Idle_NoWeapon");
    owner->SetAttackAction(CompPlayer::ATTACK_ACTION::SHEATH);
  }
  else if (condition == CompPlayer::CONDITION::WEAPON_AXE) {
    animeId = owner->GetModel()->animator.FindAnimationId("Idle_Axe");
    owner->SetAttackAction(CompPlayer::ATTACK_ACTION::AXE_IDLE);
  }
  else {
    owner->SetAttackAction(CompPlayer::ATTACK_ACTION::SWORD_IDLE);
    animeId = owner->GetModel()->animator.FindAnimationId("Idle_Sword");
  }

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerIdle::Execute(const float& elapsedTime)
{
  CompPlayer::CONDITION condition = owner->GetCondition();

  // �ړ��X�e�[�g
  if (Mathf::Dot(owner->GetInputVec()) > 0)
  {
    if (condition == CompPlayer::CONDITION::NORMAL ||
      condition == CompPlayer::CONDITION::USE_ITEM)
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
    else
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE_WEAPON);

    return;
  }

  // �㔼�g���A�j���[�V�������Ă��Ȃ��Ȃ�J�ڂł���
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // ����X�e�[�g
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // �����E�[��
    if (owner->InputWeaponSheathAndDraw())
    {
      // �ҋ@�X�e�[�g���d�؂蒼��
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);

      return;
    }

    // �U������
    if (owner->InputAttack()) {
      return;
    }
  }
}

#pragma endregion

#pragma region �ړ����

void PlayerMove::Enter()
{
  oldDashFlg = owner->InputDash();
  owner->SetAttackAction(CompPlayer::ATTACK_ACTION::SHEATH);

  int animeId = -1;
  if (oldDashFlg) {
    animeId = owner->GetModel()->animator.FindAnimationId("Run_NoWeapon");
  }
  else {
    animeId = owner->GetModel()->animator.FindAnimationId("Walk_NoWeapon");
  }

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerMove::Execute(const float& elapsedTime)
{
#pragma region �X�e�[�g�J�ڏ���
  // �ړ����͂��Ȃ���Αҋ@�X�e�[�g��
  if (Mathf::Dot(owner->GetInputVec()) == 0) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

  // �_�b�V�����͂ɕω�������΁AMove�X�e�[�g���d�؂蒼��
  if (owner->InputDash() != oldDashFlg) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
    return;
  }

  // �㔼�g���A�j���[�V�������Ă��Ȃ��Ȃ�J�ڂł���
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // ����X�e�[�g
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }
    // �U������
    if (owner->InputAttack()) {
      owner->SetCondition(CompPlayer::CONDITION::WEAPON_AXE);
      return;
    }
  }

#pragma endregion

  // �A�j���[�V�����Đ����x��ύX
  {
    float animeSpeed = owner->CalcMoveAnimeSpeed();
    owner->GetModel()->animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, animeSpeed);
  }

  // ���� 
  owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());

  // �ړ�����
  owner->Move(owner->GetInputVec());
}

void PlayerMove::Exit()
{
  if (owner->GetActionSM().GetNextState() != CompPlayer::ACTION_STATE::MOVE) {
    owner->ResetDashFlg();
  }
}

#pragma endregion

#pragma region ������

void PlayerDodge::Enter()
{
  int animeId = owner->GetModel()->animator.FindAnimationId("Dodge_Front");

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerDodge::Execute(const float& elapsedTime)
{
  // �A�j���[�V�����L�����Z������
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // �[��
    if (owner->InputWeaponSheathAndDraw() || owner->InputDash()) {
      // �[���ړ��X�e�[�g��
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
      owner->ResetDashFlg();
      return;
    }
  }
  // �A�j���[�V�������I������Αҋ@�X�e�[�g��
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region �����ړ����

void PlayerMoveWeapon::Enter()
{
  int animeId = -1;
  if (owner->GetCondition() == CompPlayer::CONDITION::WEAPON_AXE) {
    animeId = owner->GetModel()->animator.FindAnimationId("Walk_Axe");
    owner->SetAttackAction(CompPlayer::ATTACK_ACTION::AXE_IDLE);
  }
  else {
    animeId = owner->GetModel()->animator.FindAnimationId("Walk_Sword");
    owner->SetAttackAction(CompPlayer::ATTACK_ACTION::SWORD_IDLE);
  }

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerMoveWeapon::Execute(const float& elapsedTime)
{
#pragma region �X�e�[�g�J�ڏ���
  // �ړ����͂��Ȃ���Αҋ@�X�e�[�g��
  if (Mathf::Dot(owner->GetInputVec()) == 0)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

  // �㔼�g���A�j���[�V�������Ă��Ȃ��Ȃ�J�ڂł���
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // �[��
    if (owner->InputWeaponSheathAndDraw() || owner->InputDash()) {
      // �[���ړ��X�e�[�g��
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
      owner->ResetDashFlg();
      return;
    }
    // ����X�e�[�g
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }
    // �U������
    if (owner->InputAttack()) {
      return;
    }
  }

#pragma endregion

  // �A�j���[�V�����Đ����x��ύX
  {
    float animeSpeed = owner->CalcMoveAnimeSpeed();
    owner->GetModel()->animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, animeSpeed);
  }

  // ���� 
  owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());

  // �ړ�����
  owner->Move(owner->GetInputVec());
}

#pragma endregion

#pragma region �ėp�U��

void PlayerAttack::Enter()
{
  int animeId = -1;
  animeId = owner->GetModel()->animator.FindAnimationId(owner->GetAttackAnimeName());

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerAttack::Execute(const float& elapsedTime)
{
  auto animator = owner->GetModel()->animator;

  // �A�j���[�V�����L�����Z������
  if (animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // ����X�e�[�g
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // �U���h��
    if (owner->InputAttack()) {
      return;
    }
  }
  // �A�j���[�V�����I��
  else if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }


}

#pragma endregion

#pragma region ���[�v�U��

void PlayerAttackLoop::Enter()
{
  if (owner->GetActionSM().GetOldState() != CompPlayer::ACTION_STATE::ATTACK_LOOP)
  {
    loopCount = 0;
  }

  int animeId = -1;

  // ���[�v�񐔂��A0 or ������̎�
  if (loopCount % 2 == 0) {
    animeId = owner->GetModel()->animator.FindAnimationId("AxeAttack_Loop1");
  }
  // ���[�v�񐔂��A���̎�
  else {
    animeId = owner->GetModel()->animator.FindAnimationId("AxeAttack_Loop2");
  }
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);

  loopCount++;
}

void PlayerAttackLoop::Execute(const float& elapsedTime)
{
  auto animator = owner->GetModel()->animator;

  // �A�j���[�V�����L�����Z������
  if (animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // ����X�e�[�g
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // ���[�v�I�� ( ���[�v�񐔂�3�ȏ�Ȃ�A���[�v�I���U�� )
    if (owner->InputButtonY()) {
      if (loopCount >= 3)
        owner->SetAttackAction(CompPlayer::ATTACK_ACTION::AXE_LOOP_END);
      else
        owner->SetAttackAction(CompPlayer::ATTACK_ACTION::COMBO_AXE_Y3);

      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return;
    }

    // ���[�v�J��Ԃ�
    if (owner->InputButtonB()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK_LOOP);
      return;
    }
  }
  // �A�j���[�V�����I��
  else if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region �ό`�U��

void PlayerAttackTrans::Enter()
{
}

void PlayerAttackTrans::Execute(const float& elapsedTime)
{
}

#pragma endregion