#include "CompStartUpScroll.h"

void CompStartUpScroll::Start()
{
  SharedObject obj = gameObject.lock();

  sprite = obj->GetComponent<CompSprite2D>();

  normalAlpha = sprite.lock()->GetColor().w;

  // 初期化
  DirectX::XMFLOAT4 color = sprite.lock()->GetColor();
  color.w = 0.0f;
  sprite.lock()->SetColor(color);
}

void CompStartUpScroll::Update(const float& elapsedTime)
{
  if ((*displayFlag) == true) {
    startUpRate += elapsedTime * startUpSpeed;
    startUpRate = min(startUpRate, 1.0f);

    // スクロール
    sprite.lock()->SetTexPos(sprite.lock()->GetTexPos()
      + scrollSpeed * elapsedTime // スクロール量
      * (1 - startUpRate * startUpRate * startUpRate * startUpRate)); // 後半につれてスクロール量を減らす

    // 透明度 ( y=1-\frac{\operatorname{abs}\left(x\ -0.5\right)}{0.5}^{4} )
    // https://www.desmos.com/calculator/2rnqgoa6a4?lang=ja
    DirectX::XMFLOAT4 color = sprite.lock()->GetColor();
    float a = std::fabs(startUpRate - 0.5f) / 0.5f;
    color.w = normalAlpha * (1.0f - a * a * a * a);
    sprite.lock()->SetColor(color);
  }
}

void CompStartUpScroll::DrawImGui()
{
}
