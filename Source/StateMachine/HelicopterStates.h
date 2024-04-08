#pragma once

#include "State.h"
#include "Component/ModelAnimator.h"
#include "Component/CompHelicopter.h"
#include "Component/CompStraightBullet.h"

class HeliIdle :public State<CompHelicopter>
{
public:
  HeliIdle(CompHelicopter* owner) :State(owner) {}
  void Enter() override;
  void Execute(const float& elapsedTime) override;

private:
  float idleTimer = 0.0f;

  // ç≈ëÂë“ã@éûä‘
  const float IDLE_TIME_MAX = 2.0f;
  const float IDLE_TIME_MIN = 0.5f;

};

class HeliWander :public State<CompHelicopter>
{
public:
  HeliWander(CompHelicopter* owner) :State(owner) {}
  void Enter() override;
  void Execute(const float& elapsedTime) override;

};


class HeliPursuit :public State<CompHelicopter>
{
public:
  HeliPursuit(CompHelicopter* owner) :State(owner) {}
  void Execute(const float& elapsedTime)override;

private:
  const float PURSUIT_DISTANCE = 10.0f;

};

class HeliAttack :public State<CompHelicopter>
{
public:
  HeliAttack(CompHelicopter* owner);
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  DirectX::XMFLOAT3 CalcRightDir(const DirectX::XMFLOAT3& vec);

  void UpdateRotationDir(const float& elapsedTime);

  void MoveUpdate(const float& elapsedTime,const DirectX::XMFLOAT3& playerVecXZ);

  void AttackUpdate(const float& elapsedTime, const DirectX::XMFLOAT3& playerVec);

private:
  const float ATTACK_DISTANCE = 21;

  const float REVERSE_TIME_MIN = 2.0f;
  const float REVERSE_TIME_MAX = 4.0f;
  const float ATTACK_TIME_MIN = 0.2f;
  const float ATTACK_TIME_MAX = 1.8f;

  std::shared_ptr<CompStraightBullet> compBullet;

  bool  isTurnRight = false;
  float reverseTime = 0.0f;
  float reverseTimer = 0.0f;

  float attackCoolTime = ATTACK_TIME_MAX;

};

class HeliDeath :public State<CompHelicopter>
{
public:
  HeliDeath(CompHelicopter* owner) :State(owner) {}

  void Enter()override;
  void Execute(const float& elapsedTime)override;

public:
  const float ROTATION_SPEED = DirectX::XMConvertToRadians(420);
  const float GRAVITY_AFFECT = 0.4f;

};