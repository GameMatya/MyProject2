#pragma once

#include <DirectXMath.h>
#include <memory>

#include "GameObject/GameObject.h"

// 3D用当たり判定の種類
enum COLLIDER_SHAPE_3D {
  NONE = -1,
  SPHERE,
  CAPSULE,
};

//------------------------------------------------
//	 当たり判定 構造体
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
  //	 当たり判定 関数
  //------------------------------------------------
  // 球と球の交差判定
  inline bool SphereVsSphere(
    const Collider3D& other,
    const bool& isTrigger, const float& myPushRate
  );

  // 球とカプセルの交差判定
  inline bool SphereVsCapsule(
    const Collider3D& other,
    const bool& isTrigger, const float& myPushRate
  );

  // カプセルとカプセルの交差判定
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
  // 当たり判定の形状
  COLLIDER_SHAPE_3D shape;

  // 当たり判定の持ち主
  WeakObject gameObject;

  // 原点の行列
  const DirectX::XMMATRIX* parentTransform;

};
