#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "CompPlayer.h"
#include "CompWeaponBase.h"
#include "Math/OriginMath.h"

#include <memory>

class CompMagazineCoverUI :public Component
{
public:
  CompMagazineCoverUI(CompPlayer* player, bool isRight);

  // –¼‘OŽæ“¾
  const char* GetName() const override { return "MagazineUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  CompWeaponBase* weapon = nullptr;
  bool isRight = false;

  std::weak_ptr<CompSprite2D> sprite;

};
