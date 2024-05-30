#include "PlayerState.h"

void PlayerIdle::Enter()
{
  // 待機アニメーション再生  
  int animeId = owner->GetModel()->animator.FindAnimationId("Idle_NoWeapon");
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, true);
}

void PlayerIdle::Execute(const float& elapsedTime)
{
  // 移動ステート
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
#pragma region ステート遷移処理
  // 移動入力がなければ待機ステートへ
  if (Mathf::Dot(owner->GetInputVec()) == 0)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

#pragma endregion

  // アニメーション再生速度を変更
  {
    float animeSpeed = owner->CalcMoveAnimeSpeed();
    owner->GetModel()->animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, animeSpeed);
  }

  // 旋回 
  owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());

  // 移動処理
  owner->Move(owner->GetInputVec());
}

void PlayerDodge::Enter()
{
  int animeId = owner->GetModel()->animator.FindAnimationId("Dodge_Front");

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerDodge::Execute(const float& elapsedTime)
{
  // 移動入力がなければ待機ステートへ
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

}
