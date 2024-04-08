#include "AcArmStates.h"
#include "Component/CompArmoredCore.h"
#include "Component/CompGun.h"

AcArmShot::AcArmShot(CompArmoredCore* owner, const ModelAnimator::ANIM_AREA& area) :State(owner), armArea(area), animator(&owner->GetModel()->animator)
{
  // 右手か左手かを判定
  if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
    gun = dynamic_cast<CompGun*>(owner->GetRightWeapon());
  else
    gun = dynamic_cast<CompGun*>(owner->GetLeftWeapon());
}

void AcArmShot::Enter()
{
  // 銃がリロード中なら何もしない
  if (gun->CanExecute() == false) {
    // 右腕の場合
    if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
      owner->GetRightArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    // 左腕の場合
    if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
      owner->GetLeftArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    return;
  }

  animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_Enter"), false);
}

void AcArmShot::Execute(const float& elapsedTime)
{
  WeakObject target = owner->GetAttackTarget();

  // 敵の方向に向く
  if (target.expired() == false) {
    // ベクトルをXZ平面に加工
    DirectX::XMFLOAT3 targetVecNorm = owner->GetAttackTargetVec();
    targetVecNorm.y = 0.0f;
    targetVecNorm = Mathf::Normalize(targetVecNorm); // 正規化

    owner->Turn(elapsedTime, targetVecNorm, CompArmoredCore::ABRUPT_TURN_SPEED);
  }

  // アニメーションを切り替えるか確認
  bool changeAnimation = false;
  AnimeChangeCheck(changeAnimation);

  // アニメーションの変更
  if (changeAnimation == true) {
    UpdateAnimeState();
  }

  // 再生中のアニメーションがループだったら
  else if (animationState == ANIMATION_STATE::LOOP)
  {
    // 射撃済みは何もしない
    if (isShot == true)return;

    // アニメーションから攻撃のタイミングを取得する
    if (animator->CheckEvent(armArea, ANIMATION_EVENT::ATTACK)) {
      isShot = true;
      gun->Execute(target.lock().get());
    }
  }
}

void AcArmShot::Exit()
{
  animationState = 0;
}

void AcArmShot::AnimeChangeCheck(bool& changeAnime)
{
  // アニメーション終了
  if (animator->IsPlayAnimation(armArea) == false) {
    changeAnime = true;
  }
  else if (animationState == ANIMATION_STATE::LOOP) {
    // マガジンの弾数が0になったら、射撃終了
    if (gun->CanExecute() == false) {
      changeAnime = true;
      return;
    }

    // アニメーションイベントの判定
    if (animator->CheckEvent(armArea, ANIMATION_EVENT::CANCEL_ACCEPT)) {
      isShot = false;

      // 攻撃入力が無かったら射撃終了
      // 右腕の場合
      if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
        changeAnime = !owner->InputRightHand();

      // 左腕の場合
      if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
        changeAnime = !owner->InputLeftHand();
    }
  }
}

void AcArmShot::UpdateAnimeState()
{
  switch (animationState)
  {
  case ANIMATION_STATE::ENTER: // 射撃アニメーション ( ループ再生 )
    animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_Loop"), true);
    animator->SetAnimationSpeed(armArea, gun->GetAnimeSpeed());

    animationState = ANIMATION_STATE::LOOP;
    break;
  case ANIMATION_STATE::LOOP: // 射撃終了アニメーション
    animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_End"), false);

    animationState = ANIMATION_STATE::END;
    break;
  case ANIMATION_STATE::END: // ステートを抜ける
    // 右腕の場合
    if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
      owner->GetRightArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    // 左腕の場合
    if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
      owner->GetLeftArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    break;
  default: // 例外処理
    assert(!"Arm Animation State is Error !!!");
    break;
  }
}
