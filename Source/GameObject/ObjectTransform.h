#pragma once

#include "Math/OriginMath.h"
#include <memory>

class GameObject;

using WeakObject = std::weak_ptr<GameObject>;
using SharedObject = std::shared_ptr<GameObject>;

class ObjectTransform
{
public:
  ObjectTransform(GameObject* gameObject) :owner(gameObject) {}

  // 姿勢行列更新
  void UpdateTransform();

  // 回転値を加える
  void AddRotation(const DirectX::XMVECTOR& addQuaternion);

  // 親のワールド行列設定
  void SetParentTransform(SharedObject gameObject);
  void SetParentTransform(SharedObject gameObject, const DirectX::XMMATRIX* nodeTransform);

  bool HasParent() { return hasParent; }

  // 親オブジェクトの取得
  WeakObject GetParentObject() { return parentObject; }

  // 親のワールド行列取得
  const DirectX::XMMATRIX* GetParentTransform()const { return parentTransform; }

  // ワールド行列の取得
  const DirectX::XMMATRIX& GetWorldTransform()const { return WorldTransform; }

public:
  DirectX::XMFLOAT3 position = {};
  DirectX::XMFLOAT4 rotation = { 0,0,0,1 }; // 3Dモデルの場合クォータニオン
  DirectX::XMFLOAT3 scale = { 1,1,1 };

  // 各方向ベクトル
  DirectX::XMFLOAT3 right = {};
  DirectX::XMFLOAT3 up = {};
  DirectX::XMFLOAT3 forward = {};

private:
  // 持ち主
  GameObject* owner = nullptr;

  // ワールド行列
  DirectX::XMMATRIX	WorldTransform = DirectX::XMMatrixIdentity();

  // オブジェクトの親子関係用ポインタ
  WeakObject parentObject;
  const DirectX::XMMATRIX* parentTransform = nullptr;
  // 親子関係を持っているか
  bool  hasParent = false;

};