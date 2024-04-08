#include "CompMagazineCoverUI.h"

#include <algorithm>

CompMagazineCoverUI::CompMagazineCoverUI(CompPlayer* player, bool isRight) :isRight(isRight)
{
  if (isRight == true) {
    weapon = player->GetRightWeapon();
  }
  else
  {
    weapon = player->GetLeftWeapon();
  }
}

void CompMagazineCoverUI::Start()
{
  sprite = gameObject.lock()->GetComponent<CompSprite2D>();
}

void CompMagazineCoverUI::Update(const float& elapsedTime)
{
  float reload = weapon->GetReloadRate();

  float angle = 58.0f;
  if (isRight == false)
    angle = -57.0f;

  // ƒŠƒ[ƒhˆ—’†
  if (reload > 0.0f) {
    sprite.lock()->SetAngle(std::lerp(angle, 0.0f, 1.0f - reload));
  }
  else {
    sprite.lock()->SetAngle(std::lerp(angle, 0.0f, weapon->GetMagazineRate()));
  }
}
