#include "CompSightUI.h"

void CompSightUI::Start()
{
  sightSprite = gameObject.lock()->GetComponent<CompSprite2D>();
}

void CompSightUI::Update(const float& elapsedTime)
{
  if (drawTerm != nullptr) {
    DirectX::XMFLOAT4 color = sightSprite.lock()->GetColor();
    color.w = float(*drawTerm);
    sightSprite.lock()->SetColor(color);
  }

  // üŒ`•âŠÔ‚ÅŠŠ‚ç‚©‚É“®‚©‚·
  const float RETICLE_SPEED = 0.65f;
  DirectX::XMFLOAT3& position = gameObject.lock()->transform.position;
  position.x = std::lerp(position.x, (*drawPos).x, RETICLE_SPEED);
  position.y = std::lerp(position.y, (*drawPos).y, RETICLE_SPEED);
}
