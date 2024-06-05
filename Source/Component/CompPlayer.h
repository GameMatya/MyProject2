#pragma once

#include "CompCharacter.h"
#include "Graphics/TrailEffect.h"
#include "StateMachine/StateMachine.h"

class CompPlayer :public CompCharacter
{
public:
  // メインの行動
  enum class ACTION_STATE
  {
    IDLE,             // 待機

    MOVE,             // 移動
    DODGE,            // 回避

    MOVE_WEAPON,      // 抜刀状態での移動
    ATTACK,           // 攻撃
    ATTACK_LOOP,      // 攻撃
    ATTACK_TRANS,     // 変形攻撃

    DEATH,            // 死亡
    DAMAGE,           // 被ダメージ

    MAX
  };

  // プレイヤーの状況
  enum class CONDITION
  {
    NORMAL,
    USE_ITEM,
    WEAPON_AXE,
    WEAPON_SWORD,
  };

  enum class ATTACK_ACTION
  {
    EMPTY,
    SHEATH, // 納刀状態の待機

    // 斧の攻撃アクション
    AXE_IDLE,
    AXE_DASH,

    COMBO_AXE_Y1,
    COMBO_AXE_Y2,
    COMBO_AXE_Y3,

    AXE_LOOP,
    AXE_LOOP_END,

    // 剣の攻撃アクション
    SWORD_IDLE,
    COMBO_SWORD_Y1,
    COMBO_SWORD_Y2,
    COMBO_SWORD_Y3,

    COMBO_SWORD_B1,
    COMBO_SWORD_B2,
    COMBO_SWORD_B3,
  };

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override;

  // 基底クラスのMove関数をラップ
  void Move(DirectX::XMFLOAT3 direction);

  void ResetDashFlg();

  StateMachine<CompPlayer, ACTION_STATE>& GetActionSM() { return actionStateMachine; }

  ATTACK_ACTION GetAttackAction() { return attackAction; }
  std::string GetAttackAnimeName() { return attackAnimeNames[attackAction]; }
  std::array<ATTACK_ACTION, 2> GetAttackSequence() { return attackSequence[attackAction]; }
  void SetAttackAction(ATTACK_ACTION action) { attackAction = action; }

  CONDITION GetCondition() { return condition; }
  void SetCondition(CONDITION condition) { this->condition = condition; }

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  DirectX::XMFLOAT3 GetInputVec() { return inputVector; }

  bool IsRockOn() { return isRockOn; };

  bool InputDodge();
  bool InputDash();
  bool InputWeaponSheathAndDraw();
  bool InputAttack();
  bool InputButtonY();
  bool InputButtonB();
  bool InputWeaponTransform();

private:
  inline void InitAttackContainer();

  // 入力ベクトルの更新
  void UpdateInputVector();

private:
  // 先行入力受け付けフレーム
  const int EARLY_INPUT_RECEIPT_FRAME = 15;

  // 移動速度
  const float MOVE_SPEED_NO_WEAPON = 16.0f;
  const float MOVE_SPEED_DASH = 32.0f;
  const float MOVE_SPEED_USE_ITEM = 13.0f;
  const float MOVE_SPEED_AXE = 12.5f;
  const float MOVE_SPEED_SWORD = 11.5f;

  bool isRockOn = false;
  bool dashFlg = false;
  bool dashThumbFlg = false;
  bool isDrawingSword = false;

  // 攻撃派生のコンテナ 配列: 0 = Y派生 , 1 = B派生
  ATTACK_ACTION attackAction = ATTACK_ACTION::EMPTY;
  std::map<ATTACK_ACTION, std::array<ATTACK_ACTION, 2>> attackSequence;
  std::map<ATTACK_ACTION, std::string> attackAnimeNames;

  // プレイヤーの状況
  CONDITION condition = CONDITION::NORMAL;

  std::weak_ptr<CompCharacter> attackTarget;

  // スティックの入力方向
  DirectX::XMFLOAT3 inputVector = {};

  // 行動制御マシン
  StateMachine<CompPlayer, ACTION_STATE>  actionStateMachine; // メインの行動を担当
};
