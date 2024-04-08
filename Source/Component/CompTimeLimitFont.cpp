#include "CompFontTimeLimit.h"

void CompFontTimeLimit::Start()
{
  font = gameObject.lock()->GetComponent<CompFont>();
}

void CompFontTimeLimit::Update(const float& elapsedTime)
{
  // §ŒÀŠÔ‚ªˆê’è‚Ü‚Å­‚È‚­‚È‚Á‚½‚ç•¶š‚ğÔ‚­‚·‚é
  if (*timer <= CAUTION_BORDER) {
    float alpha = font.lock()->GetColor().w;
    DirectX::XMFLOAT4 color = CAUTION_COLOR;
    color.w = alpha;

    font.lock()->SetColor(color);
  }
}
