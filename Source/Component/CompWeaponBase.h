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

  // 攻撃の実行
  virtual void Execute(const GameObject* target) = 0;

  void Reload() { reloadTimer = reloadTime; }

  bool IsReload() { return (reloadTimer > 0.0f); }

  bool CanExecute() { return (bullets > 0) && (reloadTimer <= 0.0f); }

  virtual bool IsSword() = 0;

  float GetAnimeSpeed() { return animeSpeed; }

  float GetReloadRate() { return reloadTimer / reloadTime; }

  float GetMagazineRate() { return (float)bullets / (float)MAGAZINE_SIZE; }

protected:
  // 攻撃の速さ
  float animeSpeed = 1.0f;

  // マガジン容量
  const int MAGAZINE_SIZE;
  int bullets = 0;

  // リロード時間
  float reloadTime = 5.0f;
  float reloadTimer = 0.0f;

  // 攻撃判定のタグ
  COLLIDER_TAGS attackTag = COLLIDER_TAGS::ENEMY;

  // 剣の場合：刀身の表示位置、銃の場合：弾丸の発射位置
  DirectX::XMMATRIX* weaponNode = nullptr;

  // 自身の持ち主
  WeakObject owner;

};
