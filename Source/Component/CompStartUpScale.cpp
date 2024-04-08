#include "CompStartUpScale.h"
#include <imgui.h>
#include <algorithm>

void CompStartUpScale::Start()
{
  SharedObject obj = gameObject.lock();

  DirectX::XMFLOAT3& scale = gameObject.lock()->transform.scale;
  normalScale.x = scale.x;
  normalScale.y = scale.y;

  // 初期化
  scale.x = 0.0f;
  scale.y = 0.0f;

  sprite = obj->GetComponent<CompSprite2D>();

  // Spriteコンポーネントがあった場合は透明にする
  if (sprite.use_count() > 0) {
    normalAlpha = sprite.lock()->GetColor().w;

    DirectX::XMFLOAT4 color = sprite.lock()->GetColor();
    color.w = 0.0f;
    sprite.lock()->SetColor(color);
  }
}

void CompStartUpScale::Update(const float& elapsedTime)
{
  if ((*displayFlag) == true) {
    startUpRate += elapsedTime * startUpSpeed;
    startUpRate = min(startUpRate, 1.0f);
  }
  else {
    startUpRate -= elapsedTime * startUpSpeed;
    startUpRate = max(startUpRate, 0.0f);
  }

  // 大きさ
  DirectX::XMFLOAT3& scale = gameObject.lock()->transform.scale;
  scale.x = std::lerp(initScale.x, normalScale.x, startUpRate);
  scale.y = std::lerp(initScale.y, normalScale.y, startUpRate);

  // 透明度
  if (sprite.use_count() > 0) {
    DirectX::XMFLOAT4 color = sprite.lock()->GetColor();
    color.w = normalAlpha * (startUpRate * startUpRate * startUpRate); // 直前まで透明にするため乗算
    sprite.lock()->SetColor(color);
  }
}

void CompStartUpScale::DrawImGui()
{
  ImGui::InputFloat("StartUpRate", &startUpRate);

  ImGui::DragFloat("Scale_Speed", &startUpSpeed, 0.1f);
  startUpSpeed = max(startUpSpeed, 0.0f);

  if (ImGui::Button("Replay")) {
    startUpRate = 0.0f;

    DirectX::XMFLOAT3& scale = gameObject.lock()->transform.scale;
    scale.x = initScale.x;
    scale.y = initScale.y;
  }
}
