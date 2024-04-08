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
  // �ǂ̌`�󓯎m�̓����蔻�肩�𔻕ʂ���r�b�g
  int functionBit = 0;

  // �r�b�g�𗧂Ă�
  functionBit |= 1 << this->shape;
  functionBit |= 1 << other.shape;

  // �ǂ��炩�̌`�󂪋�
  if (functionBit & 1 << COLLIDER_SHAPE_3D::SPHERE) {
    // �����Е��̌`����m�F
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
  // �݂���Transform
  ObjectTransform& myTransform = gameObject.lock()->transform;
  ObjectTransform& otherTransform = other.gameObject.lock()->transform;

  // Model��Ԃł̓����蔻��̈ʒu
  DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&this->position);
  DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&other.position);

  // ���[���h��Ԃɕϊ�
  PositionA = DirectX::XMVector3TransformCoord(PositionA, *parentTransform);
  PositionB = DirectX::XMVector3TransformCoord(PositionB, *other.parentTransform);

  // ���̊Ԃ̃x�N�g�������߂�
  DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);

  // ��������
  DirectX::XMVECTOR Length = DirectX::XMVector3LengthSq(Vec);
  float length;
  DirectX::XMStoreFloat(&length, Length);

  float range = this->radius + other.radius;
  if (length < range * range)
  {
    // �����o���������Ȃ��ꍇ�A�����Ō��ʂ�Ԃ�
    if (isTrigger == true)return true;

    // �P�ʃx�N�g����
    Vec = DirectX::XMVector3Normalize(Vec);

    // �߂荞�ݗʂ����߂�
    float diff = range - sqrtf(length);
    Vec = DirectX::XMVectorScale(Vec, diff);

    // ��B�̉����o����̍��W
    DirectX::XMVECTOR VelocityB = DirectX::XMVectorScale(Vec, myPushRate);
    DirectX::XMVECTOR PushPositionB = DirectX::XMVectorAdd(PositionB, VelocityB);

    // ��A�̉����o����̍��W
    DirectX::XMVECTOR VelocityA = DirectX::XMVectorScale(Vec, 1.0f - myPushRate);
    DirectX::XMVECTOR PushPositionA = DirectX::XMVectorSubtract(PositionA, VelocityA);

    // �e�I�u�W�F�N�g������ꍇ�A���߂����ʂ�e�I�u�W�F�N�g��Ԃɕϊ�����
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

    // �����o�����ʂ𔽉f
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
