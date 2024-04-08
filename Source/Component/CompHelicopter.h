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

  // ���n�������ɌĂ΂��
  void OnLanding() override;

  // ���S�������ɌĂ΂��
  void OnDead()override;

private:
  EnemySupport support;
  StateMachine<CompHelicopter, STATE_LIST> stateMachine;
  std::shared_ptr<Effect> deathEffect = nullptr;

  int forwardAnimeId = 0;

  // �n�ʂ���̍��x
  float flyHeight = 2.6f;

};
