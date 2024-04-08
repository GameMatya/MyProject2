#pragma once

#include <memory>
#include "GameObject/GameObject.h"

// �R���|�[�l���g
class Component
{
public:
  Component() {}
  virtual ~Component() {}

  // ���O�擾
  virtual const char* GetName() const = 0;

  // ���������ۂɈ�x�������s
  virtual void Start() {}

  // ���t���[�����s
  virtual void Update(const float& elapsedTime) {}

  // �j������O�Ɏ��s
  virtual void OnDestroy() {}

  // GUI�`��
  virtual void DrawImGui() {}

  // �R���|�[�l���g�̎������ݒ�
  void SetGameObject(SharedObject gameObject) { this->gameObject = gameObject; }

  void SetActive(const bool& active) { isActive = active; }

  const bool& GetActive() const { return isActive; }

  // �R���|�[�l���g�̎�������擾
  SharedObject GetGameObject() { return this->gameObject.lock(); }

protected:
  // �R���|�[�l���g�̎�����
  WeakObject	gameObject;

  bool isActive = true;

};
