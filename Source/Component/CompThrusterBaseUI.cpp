#include "CompThrusterBaseUI.h"
#include <imgui.h>

void CompThrusterBaseUI::Start()
{
  sprite = gameObject.lock()->GetComponent<CompSprite2D>();
}

void CompThrusterBaseUI::Update(const float& elapsedTime)
{
  // オーバーヒート状態の場合、赤色で点滅する
  float alpha = sprite.lock()->GetColor().w;
  if (*paramValue <= 0.0f) {
    timer += elapsedTime * timerSpeed;

    // 元の色が白色じゃない為、R成分に1以上の値を設定する
    sprite.lock()->SetColor(
      Mathf::Lerp(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), DirectX::XMFLOAT4(5.0f, 0.0f, 0.0f, alpha), fabs(sinf(timer)))
    );
  }
  else {
    sprite.lock()->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));
  }
}

void CompThrusterBaseUI::DrawImGui()
{
  ImGui::DragFloat("TimerSpeed", &timerSpeed);
}
