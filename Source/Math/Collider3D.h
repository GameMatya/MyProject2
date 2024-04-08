#pragma once

#include <DirectXMath.h>
#include <memory>

#include "GameObject/GameObject.h"

// 3D�p�����蔻��̎��
enum COLLIDER_SHAPE_3D {
  NONE = -1,
  SPHERE,
  CAPSULE,
};

//------------------------------------------------
//	 �����蔻�� �\����
//------------------------------------------------
class Collider3D {
public:
  Collider3D(COLLIDER_SHAPE_3D shape, WeakObject gameObject, const DirectX::XMMATRIX* transform = nullptr);

  bool Collision(const Collider3D& other, const bool& isTrigger, const float& myPushRate);

  void DrawDebug(DirectX::XMFLOAT4 color);

  GameObject* GetGameObject() { return gameObject.lock().get(); }

  void SetTransform(const DirectX::XMMATRIX* transform) { parentTransform = transform; }
  const DirectX::XMMATRIX& GetTransform() { return *parentTransform; }

private:
  //------------------------------------------------
  //	 �����蔻�� �֐�
  //------------------------------------------------
  // ���Ƌ��̌�������
  inline bool SphereVsSphere(
    const Collider3D& other,
    const bool& isTrigger, const float& myPushRate
  );

  // ���ƃJ�v�Z���̌�������
  inline bool SphereVsCapsule(
    const Collider3D& other,
    const bool& isTrigger, const float& myPushRate
  );

  // �J�v�Z���ƃJ�v�Z���̌�������
  inline bool CapsuleVsCapsule(
    const Collider3D& other,
    const bool& isTrigger, const float& myPushRate
  );

public:
  DirectX::XMFLOAT3 position = {};
  float radius = 1;
  float height = 1;
  DirectX::XMFLOAT3 capsulePosition = {};

private:
  // �����蔻��̌`��
  COLLIDER_SHAPE_3D shape;

  // �����蔻��̎�����
  WeakObject gameObject;

  // ���_�̍s��
  const DirectX::XMMATRIX* parentTransform;

};
