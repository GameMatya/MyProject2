#include "CompThrusterUI.h"
#include <imgui.h>

void CompThrusterUI::Start()
{
  scrollSprite = gameObject.lock()->GetComponent<CompSprite2D>();
  initTexSizeX = scrollSprite.lock()->GetTexSize().x;
}

void CompThrusterUI::Update(const float& elapsedTime)
{
  DirectX::XMFLOAT2 texSize = DirectX::XMFLOAT2(initTexSizeX * (*scrollValue), scrollSprite.lock()->GetTexSize().y);
  scrollSprite.lock()->SetTexSize(texSize);

  // スラスター残量が3割りを切ったら赤色にする
  float alpha = scrollSprite.lock()->GetColor().w;
  if (*scrollValue <= 0.3f) {
    // 元の色が白色じゃない為、R成分に1以上の値を設定する
    scrollSprite.lock()->SetColor(DirectX::XMFLOAT4(3.0f, 0.0f, 0.0f, alpha));
  }
  else {
    scrollSprite.lock()->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));
  }
}