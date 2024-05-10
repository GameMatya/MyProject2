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

  // �p���s��X�V
  void UpdateTransform();

  // ��]�l��������
  void AddRotation(const DirectX::XMVECTOR& addQuaternion);

  // �e�̃��[���h�s��ݒ�
  void SetParentTransform(SharedObject gameObject);
  void SetParentTransform(SharedObject gameObject, const DirectX::XMMATRIX* nodeTransform);

  bool HasParent() { return hasParent; }

  // �e�I�u�W�F�N�g�̎擾
  WeakObject GetParentObject() { return parentObject; }

  // �e�̃��[���h�s��擾
  const DirectX::XMMATRIX* GetParentTransform()const { return parentTransform; }

  // ���[���h�s��̎擾
  const DirectX::XMMATRIX& GetWorldTransform()const { return WorldTransform; }

public:
  DirectX::XMFLOAT3 position = {};
  DirectX::XMFLOAT4 rotation = { 0,0,0,1 }; // 3D���f���̏ꍇ�N�H�[�^�j�I��
  DirectX::XMFLOAT3 scale = { 1,1,1 };

  // �e�����x�N�g��
  DirectX::XMFLOAT3 right = {};
  DirectX::XMFLOAT3 up = {};
  DirectX::XMFLOAT3 forward = {};

private:
  // ������
  GameObject* owner = nullptr;

  // ���[���h�s��
  DirectX::XMMATRIX	WorldTransform = DirectX::XMMatrixIdentity();

  // �I�u�W�F�N�g�̐e�q�֌W�p�|�C���^
  WeakObject parentObject;
  const DirectX::XMMATRIX* parentTransform = nullptr;
  // �e�q�֌W�������Ă��邩
  bool  hasParent = false;

};