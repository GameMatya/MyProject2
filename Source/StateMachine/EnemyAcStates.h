#pragma once

#include "State.h"
#include "Component/CompEnemyAc.h"

#include <memory>

class EnemyAcPursuit :public State<CompEnemyAC>
{
public:
  EnemyAcPursuit(CompEnemyAC* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  void UpdateReverseTimer(const float& elapsedTime);

  void UpdateMoveVec();

  void StateJudgement();

private:
  const float GUN_PROBABILITY = 35.0f;
  const float BLADE_PROBABILITY = 0.0f;
  const float JUMP_PROBABILITY = 45.0f;
  const float QB_PROBABILITY = 20.0f;

  const float REVERSE_TIME_MIN = 1.4f;
  const float REVERSE_TIME_MAX = 3.0f;
  const float SIDE_MOVE_RATE = 0.5f;

  const float BATTLE_RANGE = 10.0f;

  float reverseTimer = 0.0f;

  bool  isTurnRight = false;

};

class EnemyAcRunAway :public State<CompEnemyAC>
{
public:
  EnemyAcRunAway(CompEnemyAC* owner) :State(owner) {}
  void Execute(const float& elapsedTime) override;

private:
  void UpdateMoveVec();

  void StateJudgement();

private:
  const float BATTLE_RANGE = 7.0f;

  const float GUN_PROBABILITY = 10.0f;
  const float BLADE_PROBABILITY = 40.0f;
  const float JUMP_PROBABILITY = 20.0f;
  const float QB_PROBABILITY = 30.0f;

};

class EnemyAcInBattleRange :public State<CompEnemyAC>
{
public:
  EnemyAcInBattleRange(CompEnemyAC* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  void UpdateReverseTimer(const float& elapsedTime);

  void UpdateMoveVec();

  void StateJudgement();

private:
  const float PURSUIT_RANGE = 18.0f;
  const float RUN_AWAY_RANGE = 6.0f;

  const float REVERSE_TIME_MIN = 1.0f;
  const float REVERSE_TIME_MAX = 6.5f;
  const float FORWARD_MOVE_RATE = 0.4f;

  const float GUN_PROBABILITY = 30.0f;
  const float BLADE_PROBABILITY = 20.0f;
  const float JUMP_PROBABILITY = 25.0f;
  const float QB_PROBABILITY = 25.0f;

  float reverseTimer = 0.0f;

  bool  isTurnRight = false;
};

class EnemyAcSlashApproach :public State<CompEnemyAC>
{
public:
  EnemyAcSlashApproach(CompEnemyAC* owner) :State(owner) {}
  void Execute(const float& elapsedTime)override;

private:
  void UpdateMoveVec();

  void StateJudgement();

private:
  const float EXECUTE_RANGE = 7.0f;
  const float PURSUIT_RANGE = 18.0f;

  const float GUN_PROBABILITY = 0.0f;
  const float BLADE_PROBABILITY = 0.0f;
  const float JUMP_PROBABILITY = 0.0f;
  const float QB_PROBABILITY = 50.0f;

};