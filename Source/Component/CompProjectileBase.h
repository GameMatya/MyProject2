#pragma once

#include "Component.h"
#include "CompTriggerCollision.h"
#include "Math/OriginMath.h"
#include "Graphics/TrailEffect.h"

#include <memory>

class CompProjectileBase :public Component
{
private:
  static const int TRAIL_VOLUME = 8;

public:
  CompProjectileBase(const char* filepath, float trailRadius, float attack, float speed, float lifeTime, float invincibleTime)
    :trailPath(filepath), trailRadius(trailRadius), attackPower(attack), speed(speed), lifeTimer(lifeTime), invincibleTime(invincibleTime) { }

  // 弾オブジェクトの生成
  SharedObject CreateBullet(GameObjectManager* manager, const COLLIDER_TAGS& attackTag, const DirectX::XMFLOAT3& dir);

  // トレイルエフェクト
  void CreateTrailEffect();

  void SetMoveVec(const DirectX::XMFLOAT3& vec) { moveDir = vec; }

protected:
  // 弾の生存時間　更新
  bool UpdateLifeTimer(const float& elapsedTime);

  // 旋回処理
  void Turn(const float& elapsedTime, DirectX::XMFLOAT3 direction, float turnSpeed);

  // 移動処理
  void UpdatePosition(const float& elapsedTime);

  // 衝突判定
  bool CollideRivals();

  // ステージモデルとの当たり判定
  bool CollideStage();

  // 衝突時の処理
  virtual void OnCollision(GameObject* hitObject) = 0;

  // GameObjectに自身と同じパラメータのコンポーネントを追加
  virtual void AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir) = 0;

protected:
  std::unique_ptr<TrailEffect> trail[2] = { nullptr,nullptr };
  const char* trailPath = nullptr;
  float trailRadius = 0.2f;

  DirectX::XMFLOAT3 moveDir = {};
  DirectX::XMFLOAT3 oldPosition = {};
  float speed = 10.0f;


  float radius = 1.0f;
  float attackPower = 1.0f;
  float invincibleTime = 1.0f;
  float lifeTimer = 5.0f;
  // OnCollision関数の呼び出しを一回だけにする
  bool  isCollideOnce = false;

};
