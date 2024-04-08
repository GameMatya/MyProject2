#include "CompFontTimeLimit.h"

void CompFontTimeLimit::Start()
{
  font = gameObject.lock()->GetComponent<CompFont>();
}

void CompFontTimeLimit::Update(const float& elapsedTime)
{
  // 制限時間が一定まで少なくなったら文字を赤くする
  if (*timer <= CAUTION_BORDER) {
    float alpha = font.lock()->GetColor().w;
    DirectX::XMFLOAT4 color = CAUTION_COLOR;
    color.w = alpha;

    font.lock()->SetColor(color);
  }
}
