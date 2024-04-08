#pragma once

#include "CompCharacter.h"
#include "Graphics/TrailEffect.h"
#include "StateMachine/StateMachine.h"

class CompPlayer :public CompCharacter
{
public:
  // メインの行動
  enum class MAIN_STATE
  {
    IDLE,             // 待機

    MOVE,             // 歩行
    QUICK_BOOST,      // 急加速
    DECELERATION,     // 減速

    JUMP,             // ジャンプ
    AERIAL,           // 空中
    LANDING,          // 着地

    SLASH_APPROACH,   // 近接攻撃準備
    SLASH_EXECUTE,    // 近接攻撃実行

    SECOND_APPROACH,  // 近接攻撃準備 ( 2回目 )
    SECOND_EXECUTE,   // 近接攻撃実行 ( 2回目 )

    DEATH,            // 死亡
    DAMAGE,           // 被ダメージ

    MAX
  };

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override; 
  
  bool IsRockOn() { return isRockOn; };

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  StateMachine<CompCharacter, MAIN_STATE>& GetMainSM() { return mainStateMachine; }

private:
  // 移動入力の更新
  void UpdateInputMove();

private:
  bool isRockOn = false;

  std::weak_ptr<CompCharacter> attackTarget;

  DirectX::XMFLOAT3 inputVector = {};

  // 行動制御マシン
  StateMachine<CompCharacter, MAIN_STATE>  mainStateMachine; // メインの行動を担当
};
