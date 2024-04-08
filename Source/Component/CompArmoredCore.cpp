#include "CompArmoredCore.h"
#include "StateMachine/AcStates.h"
#include "StateMachine/AcArmStates.h"
#include "Graphics/EffectManager.h"

#include <memory>
#include <imgui.h>

CompArmoredCore::CompArmoredCore() :CompCharacter()
{
  deathEffect = EffectManager::Instance().LoadEffekseer("./Data/Effekseer/BombEnd.efk");
  assert(deathEffect != nullptr);

  boosterSE = Audio::Instance().LoadAudioSource("./Data/Audio/SE/Booster.wav");
  boosterSE->setVolume(0.4f);
}

void CompArmoredCore::Start()
{
  // 基底クラスのStart関数
  CompCharacter::Start();

  // ステートマシンにステートを追加
  {
    mainStateMachine.AddState(std::make_shared<AcIdle>(this));
    mainStateMachine.AddState(std::make_shared<AcMove>(this));
    mainStateMachine.AddState(std::make_shared<AcQuickBoost>(this));
    mainStateMachine.AddState(std::make_shared<AcDecele>(this));
    mainStateMachine.AddState(std::make_shared<AcJump>(this));
    mainStateMachine.AddState(std::make_shared<AcAerial>(this));
    mainStateMachine.AddState(std::make_shared<AcLanding>(this));
    mainStateMachine.AddState(std::make_shared<AcSlashApproach>(this, false));
    mainStateMachine.AddState(std::make_shared<AcSlashExecute>(this, false));
    mainStateMachine.AddState(std::make_shared<AcSlashApproach>(this, true));
    mainStateMachine.AddState(std::make_shared<AcSlashExecute>(this, true));
    mainStateMachine.AddState(std::make_shared<AcDeath>(this));

    // 初期ステートを設定
    mainStateMachine.ChangeState(MAIN_STATE::IDLE);
  }

  // ステートマシンにステートを追加
  {
    if (rightWeapon != nullptr) {
      armRightSM.AddState(std::make_shared<AcArmShot>(this, ModelAnimator::ANIM_AREA::ARM_RIGHT));

      // 初期ステートを設定
      armRightSM.ChangeState(ARM_STATE::NONE);
    }

    if (leftWeapon != nullptr) {
      if (leftWeapon->IsSword() == false)
        armLeftSM.AddState(std::make_shared<AcArmShot>(this, ModelAnimator::ANIM_AREA::ARM_LEFT));

      // 初期ステートを設定
      armLeftSM.ChangeState(ARM_STATE::NONE);
    }
  }

  // 開始に違和感が発生しないように初期姿勢を設定
  {
    ModelAnimator& animator = model->animator;
    animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animator.FindAnimationId("Idle"), true, 0.0f);
  }
}

void CompArmoredCore::DrawImGui()
{
  CompCharacter::DrawImGui();

  ImGui::Checkbox("BoostMode", &boostMode);

  ImGui::Separator();
  // ステートマシン
  {
    const char* states[static_cast<int>(MAIN_STATE::MAX)] = {
      "IDLE",           // 待機

      "MOVE",           // 歩行
      "QUICK_BOOST",    // 急加速
      "DECELERATION",   // 減速

      "JUMP",           // ジャンプ
      "AERIAL",         // 空中
      "LANDING",        // 着地

      "SLASH_APPROACH", // 近接攻撃追尾
      "SLASH_EXECUTE",  // 近接攻撃実行

      //"DAMAGE",         // 被ダメージ
      "DEATH",          // 死亡
    };

    MAIN_STATE state = mainStateMachine.GetCurrentState();
    ImGui::Text((std::string("MainState : ") + states[(int)state]).c_str());
  }
  ImGui::Separator();
  // ステートマシン
  {
    const char* states[static_cast<int>(ARM_STATE::MAX) + 1] = {
      "NONE",           // 待機
      "SHOT",           // 射撃
    };

    ARM_STATE state = armRightSM.GetCurrentState();
    ImGui::Text((std::string("ArmState : ") + states[(int)state + 1]).c_str());
  }
}

void CompArmoredCore::Rising(const float& elapsedTime)
{
  boostMode = true;

  // 経過フレーム
  float elapsedFrame = 60.0f * elapsedTime;
  velocity.y += RISE_SPEED * elapsedFrame;
  velocity.y = (std::min)(velocity.y, MAX_RISE_SPEED);
}

bool CompArmoredCore::LeftHandExe()
{
  if (leftWeapon->IsSword() == true) {
    mainStateMachine.ChangeState(CompArmoredCore::MAIN_STATE::SLASH_APPROACH);
    return true;
  }
  else {
    if (armLeftSM.GetCurrentState() != ARM_STATE::SHOT)
      armLeftSM.ChangeState(ARM_STATE::SHOT);
    return false;
  }
}

void CompArmoredCore::ActiveBoostMode()
{
  boostMode = true;
  // 重力の影響を弱める
  gravityAffect = BOOST_GRAVITY_AFFECT;

  boosterSE->Play(true);
}

void CompArmoredCore::DeactiveBoostMode()
{
  boostMode = false;
  // 重力の影響を通常に戻す
  gravityAffect = 1.0f;

  boosterSE->Stop();
}

bool CompArmoredCore::InputMove()
{
  return (inputVector.x != 0 || inputVector.z != 0);
}

DirectX::XMFLOAT3 CompArmoredCore::GetAttackTargetVec() const
{
  DirectX::XMFLOAT3 direction = attackTarget.lock()->transform.position - gameObject.lock()->transform.position;
  return direction;
}

DirectX::XMFLOAT3 CompArmoredCore::GetAttackTargetVecXZ() const
{
  DirectX::XMFLOAT3 targetVecNorm = GetAttackTargetVec();
  targetVecNorm.y = 0.0f;
  return Mathf::Normalize(targetVecNorm); // 正規化
}

void CompArmoredCore::OnLanding()
{
  // 近接攻撃、ダメージ、死亡、クイックブースト 時は遷移しないようにする
  if (mainStateMachine.GetCurrentState() == MAIN_STATE::DAMAGE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::QUICK_BOOST ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_EXECUTE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SECOND_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SECOND_EXECUTE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::DEATH) return;

  // ブースト移動中なら移動ステートへ
  if (boostMode == true && InputMove()) {
    mainStateMachine.ChangeState(MAIN_STATE::MOVE);
  }
  // 下方向の速力が一定以上なら着地ステートへ
  else if (velocity.y <= fallSpeed) {
    mainStateMachine.ChangeState(MAIN_STATE::LANDING);
  }
  else {
    mainStateMachine.ChangeState(MAIN_STATE::IDLE);
  }
}

void CompArmoredCore::OnDead()
{
  // 死亡ステートへ遷移
  mainStateMachine.ChangeState(MAIN_STATE::DEATH);

  if (deathEffect != nullptr)
    deathEffect->Play(
      GetGameObject()->transform.position + DirectX::XMFLOAT3(0, waistHeight, 0), // 座標 + 腰までの高さ
      0.3f // スケール
    );
}
