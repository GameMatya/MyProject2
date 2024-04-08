#include "AcStates.h"
#include "System/Input.h"
#include "Math/easing.h"

#pragma region 待機

void AcIdle::Enter()
{
  // 慣性での移動値が大きければブレーキステートへ
  if (fabs(owner->GetVelocity().x) + fabs(owner->GetVelocity().z) >= DASH_END_THRESHOLD)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::DECELERATION);
    return;
  }

  // 待機アニメーション再生  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Idle"), true, 0.5f);
  // ブーストモード解除
  owner->DeactiveBoostMode();
}

void AcIdle::Execute(const float& elapsedTime)
{
  // 移動ステート
  if (owner->InputMove())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    return;
  }

  // ジャンプステート
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // クイックブーストステート
  else if (owner->InputQuickBoost())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // 近接攻撃ステート
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // 射撃入力
  owner->InputRightHand();
}

#pragma endregion

#pragma region 移動

AcMove::AcMove(CompArmoredCore* owner) :State(owner)
{
  landSE = Audio::Instance().LoadAudioSource("./Data/Audio/SE/BoostMoveLand.wav");
}

void AcMove::Enter()
{
  // 歩行時
  if (owner->GetBoostMode() == false)
  {
    moveSpeed = owner->GetMoveSpeed();
    animeID = owner->GetModel()->animator.FindAnimationId("Walk_Forward");
  }
  // ブースト移動時
  else
  {
    landSE->Play(true);
    moveSpeed = owner->GetBoostSpeed();
    animeID = owner->GetModel()->animator.FindAnimationId("Dash_Forward");
  }
}

void AcMove::Execute(const float& elapsedTime)
{
#pragma region ステート遷移処理
  // ブーストモードに切り替わったら移動ステートを遷移し直す
  if (owner->InputBoostMode()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    return;
  }

  // クイックブーストステート
  else if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // ジャンプステート
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // 近接ステート
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // 空中ステート
  else if (owner->GetIsGround() == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
    return;
  }

#pragma endregion

  // 射撃入力
  owner->InputRightHand();

  // 移動入力がなければ待機ステートへ
  if (owner->InputMove() == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }

  // アニメーション再生速度を変更
  {
    // 歩行アニメーションが遅いので1.5倍で調整
    float animeSpeed = CalcAnimeSpeed(moveSpeed) * WALK_ANIM_PLAY_SPEED;
    owner->GetModel()->animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, animeSpeed);
  }

  // 移動 + 旋回 + アニメーション再生
  {
    // 通常時
    if (IsLookTarget() == false) {
      owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
    }
    // ロックオン時
    else {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }

    // 移動処理
    owner->Move(owner->GetInputVec(), moveSpeed);

    // アニメーション ( 移動方向に応じて4方向アニメーションを制御 )
    owner->AnimationFromVectors(owner->GetInputVec(), animeID, true);
  }
}

void AcMove::Exit()
{
  owner->GetModel()->animator.PlayAnimationBottomSub(-1);
  landSE->Stop();
}

bool AcMove::IsLookTarget()
{
  if (owner->IsRockOn())return true;

  // 攻撃対象が居ない
  if (owner->GetAttackTarget().expired())return false;

  // 腕が攻撃ステートになっている
  if (owner->GetLeftArmSM().GetCurrentState() != CompArmoredCore::ARM_STATE::NONE)return true;
  if (owner->GetRightArmSM().GetCurrentState() != CompArmoredCore::ARM_STATE::NONE)return true;

  // 攻撃対象が居る、腕が攻撃ステートじゃない
  return false;
}

float AcMove::CalcAnimeSpeed(const float& baseSpeed)
{
  // 水平移動速度
  DirectX::XMFLOAT2 horizonVelocity = { owner->GetVelocity().x,owner->GetVelocity().z };
  float speedSq = Mathf::Dot(horizonVelocity, horizonVelocity);

  // 上限は1
  return min(speedSq / baseSpeed, 1);
}

#pragma endregion

#pragma region クイックブースト

AcQuickBoost::AcQuickBoost(CompArmoredCore* owner) :State(owner)
{
  se = Audio::Instance().LoadAudioSource("./Data/Audio/SE/QuickBoost.wav");
  se->setVolume(0.42f);
}

void AcQuickBoost::Enter()
{
  // ブーストモードを起動
  owner->ActiveBoostMode();

  // 移動入力が無ければ前方向に進む
  if (owner->InputMove()) {
    moveVector = Mathf::Normalize(owner->GetInputVec());
  }
  else {
    moveVector = owner->GetGameObject()->transform.forward;
  }

  // 入力方向に合ったアニメーションを再生する
  owner->AnimationFromVectors(Mathf::Normalize(moveVector), owner->GetModel()->animator.FindAnimationId("QuickBoost_Forward"), false, 0.1f);

  se->Play(false);
}

void AcQuickBoost::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // アニメーションが終了したら別のステートに遷移
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    // 移動ステート
    if (owner->InputMove())
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
      return;
    }
    // 空中ステート
    else if (owner->GetIsGround() == false)
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
      return;
    }

    // 待機ステート
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }

  // クイックブースト時の挙動を制御 ( https://www.geogebra.org/graphing?lang=jaで作成 、式 = 1-(-x^(a)+1)^(b) )
  {
    const float a = 0.7f;
    const float b = 5.0f;
    float speedRate = 1 - powf(-powf(animator.GetAnimationRate(ModelAnimator::ANIM_AREA::BOTTOM), a) + 1, b);
    float moveSpeed = std::lerp(owner->GetBoostSpeed(), owner->GetBoostSpeed() * SPEED_RATE, speedRate);

    owner->SetVelocity(moveVector * moveSpeed);
  }
}

#pragma endregion

#pragma region 減速

void AcDecele::Enter()
{
  // ブレーキアニメーション再生  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Dash_Stopping"), true);

  currentFlame = 0;
}

void AcDecele::Execute(const float& elapsedTime)
{
  // キャンセル処理
  if (currentFlame > CANCEL_FLAME && owner->InputMove()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
  }

  // クイックブースト入力
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
  }

  // ジャンプ入力処理
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // 攻撃入力
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // 弾丸入力
  owner->InputRightHand();

  // 慣性
  float inertia = fabs(owner->GetVelocity().x) + fabs(owner->GetVelocity().z);

  // 水平の慣性が0になったら待機ステートへ
  if (inertia == 0.0f)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }
  // 慣性が通常時の移動可能速度以下になれば
  else if (inertia <= MOVABLE_SPEED) {
    // 移動入力処理
    if (owner->InputMove())
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
      return;
    }
  }
  // 慣性が大きい時
  else
  {
    // 非ロックオン時に旋回だけ行う
    if (owner->IsRockOn() == false)
      owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
  }

  currentFlame++;
}

#pragma endregion

#pragma region ジャンプ

void AcJump::Enter()
{
  // Jumpアニメーション再生  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Jump_Enter"), false);
}

void AcJump::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // ジャンプ開始アニメーションが終了したら空中ステートに遷移
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
    return;
  }

  // ジャンプ前
  if (hasJumped == false) {
    // 指定のキーフレームまで待機
    if (animator.GetCurrentAnimationSeconds(ModelAnimator::ANIM_AREA::BOTTOM) < 0.16f)return;

    // ジャンプベクトルを算出 ( 移動入力がある場合、入力方向に傾ける )
    DirectX::XMFLOAT3 jumpVector = owner->GetInputVec() * 0.5f + DirectX::XMFLOAT3(0, 1, 0);
    jumpVector = Mathf::Normalize(jumpVector);

    // ジャンプ力を適用
    owner->AddImpulse(jumpVector * owner->GetJumpSpeed());
    hasJumped = true;
  }
  // ジャンプ後
  else
  {
    // 移動
    if (owner->InputMove()) {
      if (owner->GetBoostMode() == false) {
        owner->Move(owner->GetInputVec(), owner->GetMoveSpeed());
      }
      else {
        owner->Move(owner->GetInputVec(), owner->GetBoostSpeed());
      }
      if (owner->IsRockOn() == false) {
        owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
      }
    }
    // ロックオン状態なら敵の方向を向く
    if (owner->IsRockOn() == true) {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }
  }
}

void AcJump::Exit()
{
  hasJumped = false;
}

#pragma endregion

#pragma region 空中

void AcAerial::Enter()
{
  // Jumpアニメーション再生  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Jump_Loop"), true);
}

void AcAerial::Execute(const float& elapsedTime)
{
  // クイックブースト
  if (owner->InputQuickBoost())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // 近接攻撃入力
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // 射撃入力
  owner->InputRightHand();

  // 移動+旋回
  {
    // 移動
    if (owner->InputMove()) {
      if (owner->GetBoostMode() == false) {
        owner->Move(owner->GetInputVec(), owner->GetMoveSpeed());
      }
      else {
        owner->Move(owner->GetInputVec(), owner->GetBoostSpeed());
      }
      if (owner->IsRockOn() == false) {
        owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
      }
    }
    // ロックオン状態なら敵の方向を向く
    if (owner->IsRockOn() == true) {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }
  }

  // 上昇
  if (owner->InputRise(elapsedTime)) {
    owner->Rising(elapsedTime);
  }
}

#pragma endregion

#pragma region 着地

void AcLanding::Enter()
{
  // 着地アニメーション再生  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Landing"), false);
}

void AcLanding::Execute(const float& elapsedTime)
{
  // クイックブーストステート
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region 近接攻撃

void AcSlashApproach::Enter()
{
  const char* animeId = (isSecond) ? "Second_Approach" : "Slash_Approach";

  // 相手に近づくアニメーション再生  
  owner->GetModel()->animator.PlayAnimation(
    ModelAnimator::ANIM_AREA::BOTTOM,
    owner->GetModel()->animator.FindAnimationId(animeId),
    true);

  // 攻撃対象取得
  attackTarget = owner->GetAttackTarget();

  sword = static_cast<CompSword*>(owner->GetLeftWeapon());

  owner->SetIsSlashEffect(true);

  // スラスター消費 ( 1回目のみ )
  if (isSecond == false) {
    owner->GetThrusterManager()->QuantitySubtract(owner->THRUSTER_COST_BLADE);
  }
}

void AcSlashApproach::Execute(const float& elapsedTime)
{
#pragma region ステート遷移処理
  // クイックブーストでキャンセル可能
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  CompArmoredCore::MAIN_STATE executeState = (isSecond) ? CompArmoredCore::MAIN_STATE::SECOND_EXECUTE : CompArmoredCore::MAIN_STATE::SLASH_EXECUTE;

  // 攻撃対象がいる場合
  if (attackTarget.expired() == false) {
    DirectX::XMFLOAT3 targetVec = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
    float distanceSq = Mathf::Dot(targetVec, targetVec);

    // 敵の方向に向く
    {
      owner->Turn(elapsedTime, GetAttackTargetVecXZ(), CompArmoredCore::ABRUPT_TURN_SPEED);
    }

    // 目標に近づいたら攻撃実行
    if (distanceSq <= EXECUTE_DISTANCE * EXECUTE_DISTANCE) {
      owner->GetMainSM().ChangeState(executeState);
      return;
    }
  }
  // 追跡時間がChaseTimeを越えたら近接攻撃ステートへ
  if (chaseTimer >= CHASE_TIME) {
    owner->GetMainSM().ChangeState(executeState);
    return;
  }

#pragma endregion

  // 移動処理
  {
    DirectX::XMFLOAT3 moveVector = {};
    // 攻撃対象がいる場合
    if (attackTarget.expired() == false) {
      moveVector = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
      moveVector = Mathf::Normalize(moveVector);
    }
    else
    {
      moveVector = owner->GetGameObject()->transform.forward;
    }

    owner->SetVelocity(moveVector * owner->GetBoostSpeed() * SPEED_RATE);
  }

  sword->UpdateEffect();

  chaseTimer += elapsedTime;
}

void AcSlashApproach::Exit()
{
  // 重力有効化
  if (owner->GetBoostMode())
    owner->SetGravityAffect(owner->BOOST_GRAVITY_AFFECT);
  else
    owner->SetGravityAffect(1.0f);

  owner->SetIsSlashEffect(false);

  chaseTimer = 0;
}

DirectX::XMFLOAT3 AcSlashApproach::GetAttackTargetVecXZ()
{
  DirectX::XMFLOAT3 targetVecNorm = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
  targetVecNorm.y = 0.0f;
  return Mathf::Normalize(targetVecNorm); // 正規化
}

void AcSlashExecute::Enter()
{
  const char* animeId = (isSecond) ? "Second_Execute" : "Slash_Execute";

  // 攻撃するアニメーション再生  
  ModelAnimator& animator = owner->GetModel()->animator;
  animator.PlayAnimation(
    ModelAnimator::ANIM_AREA::BOTTOM,
    animator.FindAnimationId(animeId),
    false);
  animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, 1.5f);

  owner->SetVelocity(owner->GetVelocity() * DECELE_SCALE);

  sword = static_cast<CompSword*>(owner->GetLeftWeapon());
  sword->PlayAnimation();

  // 重力無効化
  owner->SetGravityAffect(0);
}

void AcSlashExecute::Execute(const float& elapsedTime)
{
  owner->SetIsSlashEffect(false);

  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    if (owner->InputMove())
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    else
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);

    return;
  }

  // アニメーションキャンセル制御
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // クイックブーストステート
    if (owner->InputQuickBoost()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
      return;
    }

    // 2回目近接入力
    if (owner->InputLeftHand()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::SECOND_APPROACH);
      return;
    }

    // 射撃入力
    if (owner->InputRightHand()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
      return;
    }
  }

  // 攻撃判定
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::ATTACK))
  {
    sword->Execute(nullptr);
  }

  // エフェクト制御
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::EFFECT_START))
  {
    sword->UpdateEffect();
    owner->SetIsSlashEffect(true);
  }
}

void AcSlashExecute::Exit()
{
  // 重力有効化
  if (owner->GetBoostMode())
    owner->SetGravityAffect(owner->BOOST_GRAVITY_AFFECT);
  else
    owner->SetGravityAffect(1.0f);

  owner->SetIsSlashEffect(false);

  // 近接武器のクールタイム設定
  owner->GetLeftWeapon()->Reload();
}

#pragma endregion

void AcDeath::Enter()
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // 着地アニメーション再生  
  animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM,
    animator.FindAnimationId("Dead"), false);
  animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, 1.2f);
}

void AcDeath::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetGameObject()->Destroy();
  }
}
