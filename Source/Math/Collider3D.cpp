#include "Collider3D.h"
#include "Graphics/Graphics.h"

#include "OriginMath.h"

using namespace DirectX;

Collider3D::Collider3D(COLLIDER_SHAPE_3D shape, WeakObject gameObject, const DirectX::XMMATRIX* transform) :shape(shape), gameObject(gameObject), parentTransform(transform)
{
  if (transform == nullptr)
    parentTransform = &gameObject.lock()->transform.GetWorldTransform();
  else
    parentTransform = transform;
}

bool Collider3D::Collision(const Collider3D& other, const bool& isTrigger, const float& myPushRate)
{
  // どの形状同士の当たり判定かを判別するビット
  int functionBit = 0;

  // ビットを立てる
  functionBit |= 1 << this->shape;
  functionBit |= 1 << other.shape;

  // どちらかの形状が球
  if (functionBit & 1 << COLLIDER_SHAPE_3D::SPHERE) {
    // もう片方の形状を確認
    if (functionBit & 1 << COLLIDER_SHAPE_3D::CAPSULE)
      return SphereVsCapsule(other, isTrigger, myPushRate);

    else
      return SphereVsSphere(other, isTrigger, myPushRate);
  }

  if (functionBit & 1 << COLLIDER_SHAPE_3D::CAPSULE)
    return CapsuleVsCapsule(other, isTrigger, myPushRate);

  assert(!"Collision Error");
  return false;
}

void Collider3D::DrawDebug(DirectX::XMFLOAT4 color)
{
  switch (shape) {
  case COLLIDER_SHAPE_3D::SPHERE:
  {
    DirectX::XMFLOAT3 worldPos;
    {
      DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
      Position = DirectX::XMVector3TransformCoord(Position, *parentTransform);
      DirectX::XMStoreFloat3(&worldPos, Position);
    }

    Graphics::Instance().GetDebugRenderer()->DrawSphere(worldPos, radius, color);
    break;
  }
  case COLLIDER_SHAPE_3D::CAPSULE:
  {
    DirectX::XMFLOAT3 startPos;
    DirectX::XMFLOAT3 endPos;
    {
      DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
      Position = DirectX::XMVector3TransformCoord(Position, *parentTransform);
      DirectX::XMStoreFloat3(&startPos, Position);

      DirectX::XMVECTOR EndPosition = DirectX::XMLoadFloat3(&capsulePosition);
      EndPosition = DirectX::XMVector3TransformCoord(EndPosition, *parentTransform);
      DirectX::XMStoreFloat3(&endPos, EndPosition);
    }

    Graphics::Instance().GetDebugRenderer()->DrawSphere(startPos, radius, color);
    Graphics::Instance().GetDebugRenderer()->DrawSphere(endPos, radius, color);
    break;
  }
  }
}

inline bool Collider3D::SphereVsSphere(const Collider3D& other, const bool& isTrigger, const float& myPushRate)
{
  // 互いのTransform
  ObjectTransform& myTransform = gameObject.lock()->transform;
  ObjectTransform& otherTransform = other.gameObject.lock()->transform;

  // Model空間での当たり判定の位置
  DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&this->position);
  DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&other.position);

  // ワールド空間に変換
  PositionA = DirectX::XMVector3TransformCoord(PositionA, *parentTransform);
  PositionB = DirectX::XMVector3TransformCoord(PositionB, *other.parentTransform);

  // 球の間のベクトルを求める
  DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);

  // 距離判定
  DirectX::XMVECTOR Length = DirectX::XMVector3LengthSq(Vec);
  float length;
  DirectX::XMStoreFloat(&length, Length);

  float range = this->radius + other.radius;
  if (length < range * range)
  {
    // 押し出し処理しない場合、ここで結果を返す
    if (isTrigger == true)return true;

    // 単位ベクトル化
    Vec = DirectX::XMVector3Normalize(Vec);

    // めり込み量を求める
    float diff = range - sqrtf(length);
    Vec = DirectX::XMVectorScale(Vec, diff);

    // 球Bの押し出し後の座標
    DirectX::XMVECTOR VelocityB = DirectX::XMVectorScale(Vec, myPushRate);
    DirectX::XMVECTOR PushPositionB = DirectX::XMVectorAdd(PositionB, VelocityB);

    // 球Aの押し出し後の座標
    DirectX::XMVECTOR VelocityA = DirectX::XMVectorScale(Vec, 1.0f - myPushRate);
    DirectX::XMVECTOR PushPositionA = DirectX::XMVectorSubtract(PositionA, VelocityA);

    // 親オブジェクトがいる場合、求めた結果を親オブジェクト空間に変換する
    if (myTransform.GetParentObject().expired() == false)
    {
      const DirectX::XMMATRIX& invParent = DirectX::XMMatrixInverse(nullptr, myTransform.
        GetParentObject().lock()->transform.GetWorldTransform());

      PushPositionA = DirectX::XMVector3TransformCoord(PositionA, invParent);
    }
    if (otherTransform.GetParentObject().expired() == false)
    {
      const DirectX::XMMATRIX& invParent = DirectX::XMMatrixInverse(nullptr, otherTransform.
        GetParentObject().lock()->transform.GetWorldTransform());

      PushPositionB = DirectX::XMVector3TransformCoord(PositionB, invParent);
    }

    // 押し出し結果を反映
    if (myTransform.GetParentObject().expired() == true) {
      DirectX::XMStoreFloat3(&myTransform.position, PushPositionA - PositionA + DirectX::XMLoadFloat3(&myTransform.position));
    }

    if (otherTransform.GetParentObject().expired() == true) {
      DirectX::XMStoreFloat3(&otherTransform.position, PushPositionB - PositionB + DirectX::XMLoadFloat3(&otherTransform.position));
    }

    return true;
  }

  return false;
}

inline bool Collider3D::SphereVsCapsule(const Collider3D& other, const bool& isTrigger, const float& myPushRate)
{
  assert(!"Not Mounting");
  return false;
}

inline bool Collider3D::CapsuleVsCapsule(const Collider3D& other, const bool& isTrigger, const float& myPushRate)
{
  assert(!"Not Mounting");
  return false;
}
