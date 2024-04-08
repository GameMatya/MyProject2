#pragma once

#include "CompWeaponBase.h"
#include "CompProjectileBase.h"
#include "Graphics/TrailEffect.h"

#include "Audio/Audio.h"

#include <memory>

class CompSword : public CompWeaponBase
{
private:
  static const int TRAIL_VOLUME = 15;

public:
  CompSword(float effectRange, float interval, COLLIDER_TAGS attackTag, float damageVolume, const char* trailFilepath);

  // ���O�擾
  const char* GetName() const override { return "Sword"; }

  void Update(const float& elapsedTime) override;

  // �g���C���G�t�F�N�g���X�V
  void UpdateEffect();

  // �U���A�j���[�V�����Đ�
  virtual void PlayAnimation() {}

  // �U�����s
  virtual void Execute(const GameObject* target) override;

  bool IsSword() override { return true; }

protected:
  void OnCollision(GameObject* hitObject);

protected:
  float hitStopScale = 0.1f;
  float hitStopTime = 0.6f;

private:
  std::unique_ptr<TrailEffect> trailEffect;
  std::unique_ptr<AudioSource> hitSE;
  float effectRange = 1.0f;

  float attackPower = 1.0f;
  float invincibleTime = 0.3f;

};
