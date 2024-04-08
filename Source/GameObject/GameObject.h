#pragma once

#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

#include "Task/Task.h"

#include "ObjectTransform.h"

// �O���錾
class Component;
class GameObjectManager;

using WeakObject = std::weak_ptr<GameObject>;
using SharedObject = std::shared_ptr<GameObject>;

// �A�N�^�[
class GameObject : public std::enable_shared_from_this<GameObject>, public Task
{
public:
  GameObject(TASK_LAYER layer, GameObjectManager* manager) :layer(layer), objectManager(manager), transform(this) {}
  ~GameObject();

  // ���������ۂɈ�x�����Ă΂��
  void Start();

  // ���t���[���Ă΂��
  void Update(const float& elapsedTime) override;

  void OnDestroy();

  // GUI�\��
  void DrawImGui();

  // �w�肵���ʒu�Ɍ���
  void LookAt(const DirectX::XMFLOAT3& lookPosition);

  // ���g��j��
  void Destroy();

#pragma region �Q�b�^�[�E�Z�b�^�[
  // �X���b�h���C���[�̎擾
  bool GetIsDestroy() { return isDestroy; }

  // �X���b�h���C���[�̎擾
  const TASK_LAYER& GetLayer() const { return layer; }

  // �}�l�[�W���[�擾
  GameObjectManager* GetObjectManager() { return objectManager; }

  // �����o���̋����̐ݒ�
  void SetPushPower(const float& pushPower) { this->pushPower = pushPower; }
  // �����o���̋����̎擾
  const float& GetPushPower() const { return pushPower; }

  // CompModel�R���|�[�l���g�Ŏg�p
  void SetHaveModel() { haveCompModel = true; }

  // ���O�̐ݒ�
  void SetName(const char* name) { this->name = name; }
  // ���O�̎擾
  const char* GetName() const { return name.c_str(); }

  // �R���|�[�l���g�ǉ�
  template<class T, class... Args>
  std::shared_ptr<T> AddComponent(Args... args)
  {
    std::shared_ptr<T> component = std::make_shared<T>(args...);
    component->SetGameObject(shared_from_this());
    startComponents.emplace_back(component);
    return component;
  }
  // �R���|�[�l���g�擾
  template<class T>
  std::shared_ptr<T> GetComponent()
  {
    for (std::shared_ptr<Component>& component : components)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    for (std::shared_ptr<Component>& component : startComponents)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    return nullptr;
  }
  template<class T>
  std::shared_ptr<T> GetComponentConst() const
  {
    for (const std::shared_ptr<Component>& component : components)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    for (const std::shared_ptr<Component>& component : startComponents)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    return nullptr;
  }

#pragma endregion

public:
  // �p���s��
  ObjectTransform transform;

private:
  // �j���\�肩�ǂ���
  bool isDestroy = false;

  GameObjectManager* objectManager;

  std::string			name;

  // �����蔻��̉����o���̋���
  float pushPower = 1;

  // CompModel�R���|�[�l���g�������Ă��邩
  bool haveCompModel = false;

  std::vector<std::shared_ptr<Component>>	startComponents;
  std::vector<std::shared_ptr<Component>>	components;

  // �}���`�X���b�h�ł̃��C���[�ԍ�
  TASK_LAYER layer;

  // �f�o�b�O�p�ϐ�
#ifdef _DEBUG
  DirectX::XMFLOAT3 axis = { 0,1,0 }; // ��]��
  float rotate = 0; // ��]��

#endif // _DEBUG

};
