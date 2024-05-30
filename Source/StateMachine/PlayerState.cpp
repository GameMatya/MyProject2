#include "PlayerState.h"

void PlayerIdle::Enter()
{
  // �ҋ@�A�j���[�V�����Đ�  
  int animeId = owner->GetModel()->animator.FindAnimationId("Idle_NoWeapon");
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerIdle::Execute(const float& elapsedTime)
{
  // �ړ��X�e�[�g
  if (Mathf::Dot(owner->GetInputVec()) > 0)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
    return;
  }

  if (owner->InputDodge()) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
    return;
  }
}

void PlayerMove::Enter()
{
  int animeId = owner->GetModel()->animator.FindAnimationId("Walk_NoWeapon");

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerMove::Execute(const float& elapsedTime)
{
#pragma region �X�e�[�g�J�ڏ���
  // �ړ����͂��Ȃ���Αҋ@�X�e�[�g��
  if (Mathf::Dot(owner->GetInputVec()) == 0)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
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

void PlayerDodge::Enter()
{
  int animeId = owner->GetModel()->animator.FindAnimationId("Dodge_Front");

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerDodge::Execute(const float& elapsedTime)
{
  // �ړ����͂��Ȃ���Αҋ@�X�e�[�g��
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

}
