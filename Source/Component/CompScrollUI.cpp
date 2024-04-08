#include "CompScrollUI.h"
#include <imgui.h>

void CompScrollUI::Start()
{
  scrollSprite = gameObject.lock()->GetComponent<CompSprite2D>();

  if (isLoop == true) {
    initValue = scrollSprite.lock()->GetTexPos();
  }
  else {
    initValue = scrollSprite.lock()->GetTexSize();
  }
}

void CompScrollUI::Update(const float& elapsedTime)
{

  if (isLoop == true) {
    DirectX::XMFLOAT2 scroll = DirectX::XMFLOAT2(initValue +
      (scrollDirection * (*scrollValue) * scrollSpeed));
    scrollSprite.lock()->SetTexPos(scroll);
  }
  else {
    DirectX::XMFLOAT2 texSize = DirectX::XMFLOAT2(initValue *
      (DirectX::XMFLOAT2(1.0f, 1.0f) - (scrollDirection * (1.0f-*scrollValue))));

    scrollSprite.lock()->SetTexSize(texSize);
  }

}

void CompScrollUI::DrawImGui()
{
  ImGui::DragFloat("ScrollSpeed", &scrollSpeed, 0.001f);
}
