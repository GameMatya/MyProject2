#pragma once

#include "CompCharacter.h"
#include "StateMachine/StateMachine.h"
#include "CompWeaponBase.h"
#include "ThrusterManager.h"
#include "Audio/Audio.h"

class CompArmoredCore : public CompCharacter
{
public:
  // メインの行動
  enum class MAIN_STATE
  {
    IDLE,           // 待機

    MOVE,           // 歩行
    QUICK_BOOST,    // 急加速
    DECELERATION,   // 減速

    JUMP,           // ジャンプ
    AERIAL,         // 空中
    LANDING,        // 着地

    SLASH_APPROACH, // 近接攻撃準備
    SLASH_EXECUTE,  // 近接攻撃実行

    SECOND_APPROACH, // 近接攻撃準備 ( 2回目 )
    SECOND_EXECUTE,  // 近接攻撃実行 ( 2回目 )

    DEATH,          // 死亡
    DAMAGE,         // 被ダメージ

    MAX
  };

  // 腕の行動
  enum class ARM_STATE
  {
    NONE = -1, // 無し
    SHOT, // 射撃

    MAX
  };

  // ブースト移動時の重力影響度
  const float BOOST_GRAVITY_AFFECT = 0.6f;
  // 攻撃時の旋回速度
  static constexpr float ABRUPT_TURN_SPEED = DirectX::XMConvertToRadians(900);

  // 近接攻撃のスラスター消費量
  const float THRUSTER_COST_BLADE = 23.0f;
  // クイックブーストのスラスター消費量
  const float THRUSTER_COST_QB = 23.0f;
  // 上昇時のスラスターの消費速度 ( 1秒間の消費量 )
  const float THRUSTER_COST_RISE = 25.0f;

public:
  CompArmoredCore();

  virtual void Start()override;

  void DrawImGui()override;

  void Rising(const float& elapsedTime);

  // 左手入力があった際の処理
  bool LeftHandExe();

#pragma region 入力情報
  virtual void ActiveBoostMode();
  virtual void DeactiveBoostMode();
  virtual bool InputBoostMode() = 0;

  bool InputMove();
  virtual bool InputQuickBoost() = 0;
  virtual bool InputJump() = 0;
  virtual bool InputRise(const float& elapsedTime) = 0;
  virtual bool InputRightHand() = 0;
  virtual bool InputLeftHand() = 0;
#pragma endregion

#pragma region ゲッター・セッター
  void SetRightWeapon(CompWeaponBase* weaponComp) { rightWeapon = weaponComp; }
  CompWeaponBase* GetRightWeapon() { return rightWeapon; }
  void SetLeftWeapon(CompWeaponBase* weaponComp) { leftWeapon = weaponComp; }
  CompWeaponBase* GetLeftWeapon() { return leftWeapon; }

  void SetIsSlashEffect(bool isEffect) { isSlashEffect = isEffect; }
  const bool& GetIsSlashEffect()const { return isSlashEffect; }

  StateMachine<CompArmoredCore, MAIN_STATE>& GetMainSM() { return mainStateMachine; };
  StateMachine<CompArmoredCore, ARM_STATE>& GetRightArmSM() { return armRightSM; };
  StateMachine<CompArmoredCore, ARM_STATE>& GetLeftArmSM() { return armLeftSM; };

  // スティック入力値から移動ベクトルを取得
  const DirectX::XMFLOAT3& GetInputVec() const { return inputVector; }

  ThrusterManager* GetThrusterManager() { return &thrusterManager; }
  const bool& GetBoostMode()  const { return boostMode; }
  const float& GetBoostSpeed() const { return boostSpeed; }

  // ロックオンフラグ
  bool IsRockOn() const { return isRockOn; }

  // 攻撃対象のアドレス
  WeakObject GetAttackTarget() const { return attackTarget; }

  // 攻撃対象に向かうベクトルを作成
  DirectX::XMFLOAT3 GetAttackTargetVec() const;
  DirectX::XMFLOAT3 GetAttackTargetVecXZ() const;

#pragma endregion

protected:
  void OnLanding() override;

  virtual void OnDead() override;

protected:
  bool isRockOn = false;
  WeakObject attackTarget; // 攻撃対象
  CompWeaponBase* rightWeapon = nullptr;
  CompWeaponBase* leftWeapon = nullptr;
  bool isSlashEffect = false;
  std::shared_ptr<Effect> deathEffect = nullptr;

  // 移動入力ベクトル
  DirectX::XMFLOAT3 inputVector = {};

  // ブースト移動フラグ
  bool  boostMode = false;
  // ブースト移動速度
  float boostSpeed = 14.0f;
  // スラスター管理
  ThrusterManager thrusterManager;

  // 上昇スピード ( 全機体共通 )
  const float RISE_SPEED = 1.5f;
  const float MAX_RISE_SPEED = 8.0f;

  // 行動制御マシン
  StateMachine<CompArmoredCore, MAIN_STATE>  mainStateMachine; // メインの行動を担当
  StateMachine<CompArmoredCore, ARM_STATE>   armRightSM;       // 右腕の行動を担当
  StateMachine<CompArmoredCore, ARM_STATE>   armLeftSM;        // 左腕の行動を担当

  // ブーストSE
  std::unique_ptr<AudioSource> boosterSE;

};
