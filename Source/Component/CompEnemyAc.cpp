#include "CompEnemyAc.h"
#include "Scenes/SceneGame.h"
#include "GameObject/GameObjectManager.h"
#include "Math/OriginMath.h"
#include "StateMachine/EnemyAcStates.h"
#include "imgui.h"

void CompEnemyAC::Start()
{
  // 基底クラスのStart関数
  CompArmoredCore::Start();

  // パラメーター設定
  moveSpeed = 5.0f;
  boostSpeed = 14.5f;
  jumpSpeed = 14.0f;
  fallSpeed = -15.0f;
  turnSpeed = DirectX::XMConvertToRadians(400);
  MaxHealth = 5900;
  health = MaxHealth;

  // ステートマシンにステートを追加
  {
    actionSM.AddState(std::make_shared<EnemyAcPursuit>(this));
    actionSM.AddState(std::make_shared<EnemyAcRunAway>(this));
    actionSM.AddState(std::make_shared<EnemyAcInBattleRange>(this));
    actionSM.AddState(std::make_shared<EnemyAcSlashApproach>(this));

    // 初期ステートを設定
    actionSM.ChangeState(ACTION_STATE::PURSUIT);
  }

  support.Initialize(gameObject);

  isRockOn = true;
  attackTarget = support.GetPlayer();
}

void CompEnemyAC::Update(const float& elapsedTime)
{
  // ゲームの待機時間中は処理しない
  if (SceneGame::Instance()->StandbyComplete()) 
  {
    // 行動ビットをクリア
    actionFlagBit = 0;

    // 行動のクールタイム更新
    actionCoolTimer -= elapsedTime;
    shootingTimer -= elapsedTime;

    // ステートマシンの更新
    if (support.IsAlivePlayer() == true)
    {
      actionSM.Update(elapsedTime);
      mainStateMachine.Update(elapsedTime);
      armRightSM.Update(elapsedTime);
      armLeftSM.Update(elapsedTime);
    }
  }

  // 無敵時間更新
  UpdateInvincibleTimer(elapsedTime);

  // 速力処理更新
  UpdateVelocity(elapsedTime);

  // スラスター処理更新
  thrusterManager.Update(elapsedTime, isGround);
}

void CompEnemyAC::DrawImGui()
{
  CompCharacter::DrawImGui();

  int stateID = (int)actionSM.GetCurrentState();
  ImGui::InputInt("Action State", &stateID);
}

bool CompEnemyAC::InputBoostMode()
{
  boostMode = true;
  return false;
}

bool CompEnemyAC::InputQuickBoost()
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (~actionFlagBit & (1 << (int)ACTION_BIT::QUICK_BOOST))return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_QB);
  return true;
}

bool CompEnemyAC::InputJump()
{
  return actionFlagBit & (1 << (int)ACTION_BIT::JUMP);
}

bool CompEnemyAC::InputRise(const float& elapsedTime)
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (~actionFlagBit & (1 << (int)ACTION_BIT::JUMP))return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_RISE * elapsedTime);
  return true;
}

// 攻撃入力処理
bool CompEnemyAC::InputRightHand()
{
  if (shootingTimer > 0) {
    if (armRightSM.GetCurrentState() != ARM_STATE::SHOT)
      armRightSM.ChangeState(ARM_STATE::SHOT);

    return true;
  }
  else
    return false;
}

bool CompEnemyAC::InputLeftHand()
{
  if (thrusterManager.IsOverHeat() == true)return false;

  // 左手武器が銃だった場合
  if (leftWeapon != nullptr && leftWeapon->IsSword() == false) {
    if (~actionFlagBit & (1 << (int)ACTION_BIT::LEFT_ATTACK))
      return false;

    return true;
  }

  // 左手武器が近接武器だった場合
  else {
    if (~actionFlagBit & (1 << (int)ACTION_BIT::LEFT_ATTACK))return false;
    if (thrusterManager.IsOverHeat() == true)return false;
    if (leftWeapon->CanExecute() == false)return false;

    thrusterManager.QuantitySubtract(THRUSTER_COST_BLADE);
    ActiveBoostMode();
  }

  return true;
}

void CompEnemyAC::SetInputMoveVec(const DirectX::XMFLOAT3& inputMove)
{
  // 前フレームの入力方向と次の入力方向の差が大きいほど、
  // 線形補間の補間率が小さくなる ( 敵の動きを自然にする為 )
  float applyRate = Mathf::Dot(inputVector, inputMove) + 1.0f;
  applyRate /= 2.0f;
  inputVector = Mathf::Lerp(inputVector, inputMove, applyRate);
}

void CompEnemyAC::RandomAction(const float& gunRate, const float& bladeRate, const float& jumpRate, const float& dushRate)
{
  if (actionCoolTimer > 0)return;

  float random = Mathf::RandomRange(0, 100);

  // クールタイムを設定
  actionCoolTimer = Mathf::RandomRange(ACTION_COOL_TIME_MIN, ACTION_COOL_TIME_MAX);

  // 射撃
  if (random < gunRate) {
    InputShot();
    return;
  }
  // 近接攻撃
  if (random < bladeRate + gunRate) {
    if (mainStateMachine.GetCurrentState() != MAIN_STATE::SLASH_EXECUTE) {
      actionSM.ChangeState(ACTION_STATE::SLASH_APPROACH);
      return;
    }
  }

  // ジャンプ
  if (random < jumpRate + bladeRate + gunRate) {
    ActionBitStand(ACTION_BIT::JUMP);
    return;
  }
  // クイックブースト
  if (random < dushRate + jumpRate + bladeRate + gunRate) {
    ActionBitStand(ACTION_BIT::QUICK_BOOST);
    return;
  }
}

void CompEnemyAC::InputShot()
{
  // すでに射撃中だった場合は、何もしない
  if (shootingTimer > 0)return;

  // 射撃時間を設定
  shootingTimer = Mathf::RandomRange(SHOOTING_TIME_MIN, SHOOTING_TIME_MAX);
}

void CompEnemyAC::OnDead()
{
  CompArmoredCore::OnDead();

  ScoreManager::Instance().AddPoint(SCORE_ENEMY_AC);
}
