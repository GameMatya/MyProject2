#include "ObjectTransform.h"
#include "GameObject.h"

void ObjectTransform::UpdateTransform()
{
  // ワールド行列の更新 
  DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
  DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
  DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

  WorldTransform = S * R * T;

  // 親子関係がある場合
  if (parentTransform != nullptr) {
    // 親のワールド行列を掛ける ( 親を中心とする空間 → 世界の原点を中心とする空間 )
    WorldTransform = DirectX::XMMatrixMultiply(WorldTransform, *parentTransform);
  }

  // 回転行列から各方向を取得
  DirectX::XMStoreFloat3(&right, R.r[0]);
  DirectX::XMStoreFloat3(&up, R.r[1]);
  DirectX::XMStoreFloat3(&forward, R.r[2]);
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

