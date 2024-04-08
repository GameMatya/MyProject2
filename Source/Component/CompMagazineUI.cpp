#include "CompMagazineUI.h"

CompMagazineUI::CompMagazineUI(CompPlayer* player, bool isRight) :isRight(isRight)
{
  if (isRight == true) {
    weapon = player->GetRightWeapon();
  }
  else
  {
    weapon = player->GetLeftWeapon();
  }
}

void CompMagazineUI::Start()
{
  sprite = gameObject.lock()->GetComponent<CompSprite2D>();
}

void CompMagazineUI::Update(const float& elapsedTime)
{
  if (weapon->CanExecute()) {
    sprite.lock()->SetColor(DEFAULT_COLOR);
  }
  else {
    sprite.lock()->SetColor(RELOAD_COLOR);
  }
}
