#pragma once

#include "CompArmoredCore.h"
#include "EnemySupport.h"
#include "StateMachine/StateMachine.h"

class CompHelicopter :public CompCharacter
{
public:
  enum class STATE_LIST
  {
    IDLE,
    WANDER,
    PURSUIT,
    ATTACK,
    DEAD,

    MAX
  };

public:
  void Start()override;

  void Update(const float& elapsedTime)override;

  void DrawImGui()override;

  const char* GetName() const { return "Helicopter"; }

  StateMachine<CompHelicopter, STATE_LIST>* GetStateMachine() { return &stateMachine; }

  int GetForwardAnimeID() { return forwardAnimeId; }

  EnemySupport* GetSupport() { return &support; }

private:
  void CalcPositionY();

  // 着地した時に呼ばれる
  void OnLanding() override;

  // 死亡した時に呼ばれる
  void OnDead()override;

private:
  EnemySupport support;
  StateMachine<CompHelicopter, STATE_LIST> stateMachine;
  std::shared_ptr<Effect> deathEffect = nullptr;

  int forwardAnimeId = 0;

  // 地面からの高度
  float flyHeight = 2.6f;

};
