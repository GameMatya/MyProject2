#pragma once

#include "Component.h"
#include "CompTriggerCollision.h"
#include "Math/OriginMath.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/AcArmStates.h"

#include <memory>

class CompArmoredCore;

class CompWeaponBase : public Component
{
public:
  CompWeaponBase(int magazineSize, float reloadTime, float speed, COLLIDER_TAGS tag)
    :MAGAZINE_SIZE(magazineSize), bullets(magazineSize), reloadTime(reloadTime), animeSpeed(speed), attackTag(tag) {}

  virtual void Start()override;

  void Update(const float& elapsedTime);

  // �U���̎��s
  virtual void Execute(const GameObject* target) = 0;

  void Reload() { reloadTimer = reloadTime; }

  bool IsReload() { return (reloadTimer > 0.0f); }

  bool CanExecute() { return (bullets > 0) && (reloadTimer <= 0.0f); }

  virtual bool IsSword() = 0;

  float GetAnimeSpeed() { return animeSpeed; }

  float GetReloadRate() { return reloadTimer / reloadTime; }

  float GetMagazineRate() { return (float)bullets / (float)MAGAZINE_SIZE; }

protected:
  // �U���̑���
  float animeSpeed = 1.0f;

  // �}�K�W���e��
  const int MAGAZINE_SIZE;
  int bullets = 0;

  // �����[�h����
  float reloadTime = 5.0f;
  float reloadTimer = 0.0f;

  // �U������̃^�O
  COLLIDER_TAGS attackTag = COLLIDER_TAGS::ENEMY;

  // ���̏ꍇ�F���g�̕\���ʒu�A�e�̏ꍇ�F�e�ۂ̔��ˈʒu
  DirectX::XMMATRIX* weaponNode = nullptr;

  // ���g�̎�����
  WeakObject owner;

};
