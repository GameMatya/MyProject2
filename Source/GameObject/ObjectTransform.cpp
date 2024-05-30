#include "ObjectTransform.h"
#include "GameObject.h"

void ObjectTransform::UpdateTransform()
{
  // ���[���h�s��̍X�V 
  DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
  RotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
  DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

  WorldTransform = S * RotationMatrix * T;

  // �e�q�֌W������ꍇ
  if (parentTransform != nullptr) {
    // �e�̃��[���h�s����|���� ( �e�𒆐S�Ƃ����� �� ���E�̌��_�𒆐S�Ƃ����� )
    WorldTransform = DirectX::XMMatrixMultiply(WorldTransform, *parentTransform);
  }

  // ��]�s�񂩂�e�������擾
  DirectX::XMStoreFloat3(&right, RotationMatrix.r[0]);
  DirectX::XMStoreFloat3(&up, RotationMatrix.r[1]);
  DirectX::XMStoreFloat3(&forward, RotationMatrix.r[2]);
}

void ObjectTransform::SetParentTransform(SharedObject gameObject)
{
  hasParent = true;
  parentObject = gameObject;
  parentTransform = &gameObject->transform.GetWorldTransform();
}

void ObjectTransform::SetParentTransform(SharedObject gameObject, const DirectX::XMMATRIX* nodeTransform)
{
  hasParent = true;
  parentObject = gameObject;
  parentTransform = nodeTransform;
}

void ObjectTransform::AddRotation(const DirectX::XMVECTOR& addQuaternion)
{
  DirectX::XMStoreFloat4(&rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&rotation), addQuaternion));
}

