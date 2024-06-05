#include "PlayerState.h"

#pragma region 待機状態

void PlayerIdle::Enter()
{
  // 待機アニメーション再生  
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

  // 移動ステート
  if (Mathf::Dot(owner->GetInputVec()) > 0)
  {
    if (condition == CompPlayer::CONDITION::NORMAL ||
      condition == CompPlayer::CONDITION::USE_ITEM)
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
    else
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE_WEAPON);

    return;
  }

  // 上半身がアニメーションしていないなら遷移できる
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // 回避ステート
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // 抜刀・納刀
    if (owner->InputWeaponSheathAndDraw())
    {
      // 待機ステートを仕切り直す
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);

      return;
    }

    // 攻撃入力
    if (owner->InputAttack()) {
      return;
    }
  }
}

#pragma endregion

#pragma region 移動状態

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
#pragma region ステート遷移処理
  // 移動入力がなければ待機ステートへ
  if (Mathf::Dot(owner->GetInputVec()) == 0) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

  // ダッシュ入力に変化があれば、Moveステートを仕切り直す
  if (owner->InputDash() != oldDashFlg) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
    return;
  }

  // 上半身がアニメーションしていないなら遷移できる
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // 回避ステート
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }
    // 攻撃入力
    if (owner->InputAttack()) {
      owner->SetCondition(CompPlayer::CONDITION::WEAPON_AXE);
      return;
    }
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

void PlayerMove::Exit()
{
  if (owner->GetActionSM().GetNextState() != CompPlayer::ACTION_STATE::MOVE) {
    owner->ResetDashFlg();
  }
}

#pragma endregion

#pragma region 回避状態

void PlayerDodge::Enter()
{
  int animeId = owner->GetModel()->animator.FindAnimationId("Dodge_Front");

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerDodge::Execute(const float& elapsedTime)
{
  // アニメーションキャンセル制御
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // 納刀
    if (owner->InputWeaponSheathAndDraw() || owner->InputDash()) {
      // 納刀移動ステートへ
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
      owner->ResetDashFlg();
      return;
    }
  }
  // アニメーションが終了すれば待機ステートへ
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region 抜刀移動状態

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
#pragma region ステート遷移処理
  // 移動入力がなければ待機ステートへ
  if (Mathf::Dot(owner->GetInputVec()) == 0)
  {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }

  // 上半身がアニメーションしていないなら遷移できる
  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::UPPER) == false) {
    // 納刀
    if (owner->InputWeaponSheathAndDraw() || owner->InputDash()) {
      // 納刀移動ステートへ
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::MOVE);
      owner->ResetDashFlg();
      return;
    }
    // 回避ステート
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }
    // 攻撃入力
    if (owner->InputAttack()) {
      return;
    }
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

#pragma endregion

#pragma region 汎用攻撃

void PlayerAttack::Enter()
{
  int animeId = -1;
  animeId = owner->GetModel()->animator.FindAnimationId(owner->GetAttackAnimeName());

  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);
}

void PlayerAttack::Execute(const float& elapsedTime)
{
  auto animator = owner->GetModel()->animator;

  // アニメーションキャンセル制御
  if (animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // 回避ステート
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // 攻撃派生
    if (owner->InputAttack()) {
      return;
    }
  }
  // アニメーション終了
  else if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }


}

#pragma endregion

#pragma region ループ攻撃

void PlayerAttackLoop::Enter()
{
  if (owner->GetActionSM().GetOldState() != CompPlayer::ACTION_STATE::ATTACK_LOOP)
  {
    loopCount = 0;
  }

  int animeId = -1;

  // ループ回数が、0 or 偶数回の時
  if (loopCount % 2 == 0) {
    animeId = owner->GetModel()->animator.FindAnimationId("AxeAttack_Loop1");
  }
  // ループ回数が、奇数回の時
  else {
    animeId = owner->GetModel()->animator.FindAnimationId("AxeAttack_Loop2");
  }
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animeId, false);

  loopCount++;
}

void PlayerAttackLoop::Execute(const float& elapsedTime)
{
  auto animator = owner->GetModel()->animator;

  // アニメーションキャンセル制御
  if (animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // 回避ステート
    if (owner->InputDodge()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::DODGE);
      return;
    }

    // ループ終了 ( ループ回数が3以上なら、ループ終了攻撃 )
    if (owner->InputButtonY()) {
      if (loopCount >= 3)
        owner->SetAttackAction(CompPlayer::ATTACK_ACTION::AXE_LOOP_END);
      else
        owner->SetAttackAction(CompPlayer::ATTACK_ACTION::COMBO_AXE_Y3);

      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return;
    }

    // ループ繰り返し
    if (owner->InputButtonB()) {
      owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK_LOOP);
      return;
    }
  }
  // アニメーション終了
  else if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetActionSM().ChangeState(CompPlayer::ACTION_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region 変形攻撃

void PlayerAttackTrans::Enter()
{
}

void PlayerAttackTrans::Execute(const float& elapsedTime)
{
}

#pragma endregion