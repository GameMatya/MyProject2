#pragma once

#include "State.h"
#include "Component/CompPlayer.h"

#include <memory>

class PlayerIdle :public State<CompPlayer>
{
public:
  PlayerIdle(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class PlayerMove :public State<CompPlayer>
{
public:
  PlayerMove(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;
  void Exit()override;

private:
  bool oldDashFlg = false;

};

class PlayerDodge :public State<CompPlayer>
{
public:
  PlayerDodge(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class PlayerMoveWeapon :public State<CompPlayer>
{
public:
  PlayerMoveWeapon(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class PlayerAttack:public  State<CompPlayer>
{
public:
  PlayerAttack(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class PlayerAttackLoop:public  State<CompPlayer>
{
public:
  PlayerAttackLoop(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  int loopCount = 0;

};

class PlayerAttackTrans:public  State<CompPlayer>
{
public:
  PlayerAttackTrans(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};
