#pragma once

#include "CompProjectileBase.h"

class CompStraightBullet :public CompProjectileBase
{
public:
  using CompProjectileBase::CompProjectileBase;

  // –¼‘Oæ“¾
  const char* GetName() const override { return "Straight Bullet"; }

  void Update(const float& elapsedTime)override;

private:
  // Õ“Ë‚Ìˆ—
  void OnCollision(GameObject* hitObject) override;

  void AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir)override;

};
