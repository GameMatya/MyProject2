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

// 開始処理
void GameObject::Start()
{
  for (std::shared_ptr<Component>& component : startComponents)
  {
    component->Start();
    components.emplace_back(component);
  }
  startComponents.clear();
}

// 更新
void GameObject::Update(const float& elapsedTime)
{
  // 親が設定されている場合
  if (transform.HasParent() == true) {
    // 親オブジェクトが破棄されていたら、自身を破棄
    if (transform.GetParentObject().expired() == true)
      Destroy();
  }

  for (std::shared_ptr<Component>& component : components)
  {
    // 自身が破棄予定なら更新しない
    if (isDestroy == true)return;
    // アクティブじゃなかったら無視
    if (component->GetActive() == false)continue;
    component->Update(elapsedTime);
  }

  // CompModelを所有していなかったら
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
  // 名前
  {
    char buffer[1024];
    ::strncpy_s(buffer, sizeof(buffer), GetName(), sizeof(buffer));
    if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
    {
      SetName(buffer);
    }
  }

  // トランスフォーム
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

    // 回転軸 ( 正規化 )
    if (ImGui::DragFloat3("RotationAxis", &axis.x, 0.01f, 0, 1)) {
      rotate = 0;
    }
    float normalize = axis.x + axis.y + axis.z;
    axis /= normalize;

    // 回転量
    rotate = 0;
    ImGui::SliderFloat("Rotate", &rotate, -0.1f, 0.1f);
    {
      DirectX::XMVECTOR Axis = DirectX::XMLoadFloat3(&axis);
      DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);
      DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(Axis, rotate);
      DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionMultiply(Rotation, Quaternion));
    }

    // 回転
    {
      ImGui::DragFloat3("EulerAngle", &transform.rotation.x, DirectX::XM_PI * 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
    }
  }

  // コンポーネント
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
