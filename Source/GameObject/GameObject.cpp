#include <imgui.h>
#include <numbers>

#include "GameObject.h"
#include "GameObjectManager.h"
#include "Graphics/Graphics.h"

#include "Component/Component.h"
#include "Component/CompCollision.h"
#include "Component/CompTriggerCollision.h"

#include "System/Logger.h"

GameObject::~GameObject()
{
  Logger::Print(("Deleted" + name + "\n\n").c_str());
}

// �J�n����
void GameObject::Start()
{
  for (std::shared_ptr<Component>& component : startComponents)
  {
    component->Start();
    components.emplace_back(component);
  }
  startComponents.clear();
}

// �X�V
void GameObject::Update(const float& elapsedTime)
{
  // �e���ݒ肳��Ă���ꍇ
  if (transform.HasParent() == true) {
    // �e�I�u�W�F�N�g���j������Ă�����A���g��j��
    if (transform.GetParentObject().expired() == true)
      Destroy();
  }

  for (std::shared_ptr<Component>& component : components)
  {
    // ���g���j���\��Ȃ�X�V���Ȃ�
    if (isDestroy == true)return;
    // �A�N�e�B�u����Ȃ������疳��
    if (component->GetActive() == false)continue;
    component->Update(elapsedTime);
  }

  // CompModel�����L���Ă��Ȃ�������
  if (haveCompModel == false)
    transform.UpdateTransform();
}

void GameObject::OnDestroy()
{
  for (std::shared_ptr<Component>& component : components)
  {
    component->OnDestroy();
  }
}

void GameObject::DrawImGui()
{
#ifdef _DEBUG
  // ���O
  {
    char buffer[1024];
    ::strncpy_s(buffer, sizeof(buffer), GetName(), sizeof(buffer));
    if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
      SetName(buffer);
    }
  }

  // �g�����X�t�H�[��
  if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
    ImGui::Spacing();
    ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f, 0);
    if (ImGui::DragFloat("AllScale", &transform.scale.x, 0.01f, 0)) {
      transform.scale.y = transform.scale.x;
      transform.scale.z = transform.scale.x;
    }
    ImGui::Spacing();
    ImGui::TextColored({ 1,1,0,1 }, "Rotation %.4f %.4f %.4f %.4f", transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

    // ��]�� ( ���K�� )
    if (ImGui::DragFloat3("RotationAxis", &axis.x, 0.01f, 0, 1)) {
      rotate = 0;
    }
    float normalize = axis.x + axis.y + axis.z;
    axis /= normalize;

    // ��]��
    rotate = 0;
    ImGui::SliderFloat("Rotate", &rotate, -0.1f, 0.1f);
    {
      DirectX::XMVECTOR Axis = DirectX::XMLoadFloat3(&axis);
      DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);
      DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, rotate);
      DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(Rotation, Quaternion));
    }

    // ��]
    {
      ImGui::DragFloat3("EulerAngle", &transform.rotation.x, DirectX::XM_PI * 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
    }
  }

  // �R���|�[�l���g
  for (std::shared_ptr<Component>& component : components)
  {
    if (component->GetActive() == false)continue;

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
    {
      component->DrawImGui();
    }
  }
#endif // _DEBUG
}

void GameObject::LookAt(const DirectX::XMFLOAT3& lookPosition)
{
  transform.rotation = Mathf::LookAt(transform.position, lookPosition);
}

void GameObject::Destroy()
{
  objectManager->Remove(shared_from_this());
  isDestroy = true;
}
