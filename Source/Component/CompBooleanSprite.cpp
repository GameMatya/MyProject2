#include "CompBooleanSprite.h"
#include "imgui.h"
#include <algorithm>

void CompBooleanSprite::Start()
{
  sprite = gameObject.lock()->GetComponent<CompSprite2D>();
  displayValue = sprite.lock()->GetColor().w;
}

void CompBooleanSprite::Update(const float& elapsedTime)
{
  // ƒtƒ‰ƒO‚ªTrue‚ÌŽž‚Í‰ÁŽZAFalse‚ÍŒ¸ŽZ
  displayValue += DISPLAY_SPEED * (*displayFlag ? 1.0f : -1.0f) * elapsedTime;
  displayValue = std::clamp(displayValue, 0.0f, 1.0f);

  DirectX::XMFLOAT4 color = sprite.lock()->GetColor();
  color.w = displayValue;
  sprite.lock()->SetColor(color);
}

