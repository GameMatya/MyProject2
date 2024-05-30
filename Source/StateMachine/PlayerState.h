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

};

class PlayerDodge :public State<CompPlayer>
{
public:
  PlayerDodge(CompPlayer* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

