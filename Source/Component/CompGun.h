#pragma once

#include "CompWeaponBase.h"
#include "CompProjectileBase.h"

#include <memory>

class CompGun : public CompWeaponBase
{
public:
  CompGun(int magazineSize, float reloadTime, float animeSpeed, COLLIDER_TAGS attackTag, std::shared_ptr<CompProjectileBase> bullet)
    : bulletComp(bullet), CompWeaponBase(magazineSize, reloadTime, animeSpeed, attackTag) {}

  // ���O�擾
  const char* GetName() const override { return "Gun"; }

  // �e�̔���
  void Execute(const GameObject* target) override;

  void DrawImGui()override;

  bool IsSword()override { return false; };

  bool& GetExecuteFlag() { return executeFlag; }

private:
  bool executeFlag = false;

  // �e
  std::shared_ptr<CompProjectileBase> bulletComp;

};
