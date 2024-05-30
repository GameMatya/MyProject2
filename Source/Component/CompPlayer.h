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

    MOVE,             // 歩行
    //DASH,             // 走り
    DODGE,            // 回避



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

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override;

  // 基底クラスのMove関数をラップ
  void Move(DirectX::XMFLOAT3 direction);

  bool IsRockOn() { return isRockOn; };

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  StateMachine<CompPlayer, ACTION_STATE>& GetActionSM() { return actionStateMachine; }

  DirectX::XMFLOAT3 GetInputVec() { return inputVector; }

  bool InputDodge();

private:
  // 入力ベクトルの更新
  void UpdateInputVector();

private:
  // 移動速度
  const float MOVE_SPEED_NO_WEAPON  = 8.0f;
  const float MOVE_SPEED_USE_ITEM  = 6.0f;
  const float MOVE_SPEED_AXE        = 6.0f;
  const float MOVE_SPEED_SWORD      = 4.8f;

  bool isRockOn = false;

  // プレイヤーの状況
  CONDITION condition = CONDITION::NORMAL;

  std::weak_ptr<CompCharacter> attackTarget;

  // スティックの入力方向
  DirectX::XMFLOAT3 inputVector = {};

  // 行動制御マシン
  StateMachine<CompPlayer, ACTION_STATE>  actionStateMachine; // メインの行動を担当
};
