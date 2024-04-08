#pragma once

#include "CompProjectileBase.h"

class CompStraightBullet :public CompProjectileBase
{
public:
  using CompProjectileBase::CompProjectileBase;

  // 名前取得
  const char* GetName() const override { return "Straight Bullet"; }

  void Update(const float& elapsedTime)override;

private:
  // 衝突時の処理
  void OnCollision(GameObject* hitObject) override;

  void AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir)override;

};
