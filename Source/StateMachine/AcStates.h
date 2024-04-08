#pragma once

#include "State.h"
#include "Component/CompArmoredCore.h"
#include "Component/CompSword.h"
#include "Graphics/TrailEffect.h"
#include "Audio/Audio.h"

#include <memory>

class AcIdle :public State<CompArmoredCore>
{
public:
  AcIdle(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

public:
  // ブレーキアニメーションに遷移する条件
  const float DASH_END_THRESHOLD = 14.0f;
  // 旋回速度 ( 1秒間に回転する量 )
  const float TURN_SPEED = DirectX::XMConvertToRadians(700);

};

class AcMove :public State<CompArmoredCore>
{
public:
  AcMove(CompArmoredCore* owner);
  void Enter()override;
  void Execute(const float& elapsedTime)override;
  void Exit()override;

private:
  float CalcAnimeSpeed(const float& baseSpeed);
  bool  IsLookTarget();

private:
  // 歩行アニメーションの再生速度
  const float WALK_ANIM_PLAY_SPEED = 1.5f;

  float moveSpeed = 0;
  int   animeID = -1;
  std::unique_ptr<AudioSource> landSE;

};

class AcQuickBoost :public State<CompArmoredCore>
{
public:
  AcQuickBoost(CompArmoredCore* owner);
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  // 移動ベクトル
  DirectX::XMFLOAT3 moveVector = {};

  // ブースト移動速度の倍率
  const float SPEED_RATE = 1.8f;

  std::unique_ptr<AudioSource> se;
};

class AcDecele :public State<CompArmoredCore>
{
public:
  AcDecele(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

private:
  const float MOVABLE_SPEED = 6.0f;
  const UINT CANCEL_FLAME = 5;

  UINT currentFlame = 0;

};

class AcJump :public State<CompArmoredCore>
{
public:
  AcJump(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;
  void Exit()override;

private:
  bool hasJumped = false;

};

class AcAerial :public State<CompArmoredCore>
{
public:
  AcAerial(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class AcLanding :public State<CompArmoredCore>
{
public:
  AcLanding(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class AcSlashApproach :public State<CompArmoredCore>
{
public:
  AcSlashApproach(CompArmoredCore* owner,bool isSecond) :State(owner), isSecond(isSecond) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;
  void Exit()override;

private:
  DirectX::XMFLOAT3 GetAttackTargetVecXZ();

private:
  // ブースト移動速度の倍率
  const float SPEED_RATE = 1.4f;
  // 敵を追いかける時間
  const float CHASE_TIME = 1.0f;
  // 攻撃を実行する距離
  const float EXECUTE_DISTANCE = 2.0f;

  WeakObject attackTarget;

  CompSword* sword = nullptr;
  float chaseTimer = 0.0f;

  // 2撃目フラグ
  bool isSecond = false;
};

class AcSlashExecute :public State<CompArmoredCore>
{
public:
  AcSlashExecute(CompArmoredCore* owner, bool isSecond) :State(owner), isSecond(isSecond) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;
  void Exit()override;

private:
  // 減速の強さ
  const float DECELE_SCALE = 0.2f;

  CompSword* sword = nullptr;

  // 2撃目フラグ
  bool isSecond = false;

};

class AcStun :public State<CompArmoredCore>
{
public:
  AcStun(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};

class AcDeath :public State<CompArmoredCore>
{
public:
  AcDeath(CompArmoredCore* owner) :State(owner) {}
  void Enter()override;
  void Execute(const float& elapsedTime)override;

};
