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

  // �e�I�u�W�F�N�g�̐���
  SharedObject CreateBullet(GameObjectManager* manager, const COLLIDER_TAGS& attackTag, const DirectX::XMFLOAT3& dir);

  // �g���C���G�t�F�N�g
  void CreateTrailEffect();

  void SetMoveVec(const DirectX::XMFLOAT3& vec) { moveDir = vec; }

protected:
  // �e�̐������ԁ@�X�V
  bool UpdateLifeTimer(const float& elapsedTime);

  // ���񏈗�
  void Turn(const float& elapsedTime, DirectX::XMFLOAT3 direction, float turnSpeed);

  // �ړ�����
  void UpdatePosition(const float& elapsedTime);

  // �Փ˔���
  bool CollideRivals();

  // �X�e�[�W���f���Ƃ̓����蔻��
  bool CollideStage();

  // �Փˎ��̏���
  virtual void OnCollision(GameObject* hitObject) = 0;

  // GameObject�Ɏ��g�Ɠ����p�����[�^�̃R���|�[�l���g��ǉ�
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
  // OnCollision�֐��̌Ăяo������񂾂��ɂ���
  bool  isCollideOnce = false;

};
