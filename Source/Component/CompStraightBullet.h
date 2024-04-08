#pragma once

#include "CompProjectileBase.h"

class CompStraightBullet :public CompProjectileBase
{
public:
  using CompProjectileBase::CompProjectileBase;

  // ���O�擾
  const char* GetName() const override { return "Straight Bullet"; }

  void Update(const float& elapsedTime)override;

private:
  // �Փˎ��̏���
  void OnCollision(GameObject* hitObject) override;

  void AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir)override;

};
