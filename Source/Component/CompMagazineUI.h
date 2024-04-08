#pragma once

#include "Component.h"
#include "CompSprite2D.h"
#include "CompPlayer.h"
#include "CompWeaponBase.h"
#include "Math/OriginMath.h"

#include <memory>

class CompMagazineUI :public Component
{
public:
  CompMagazineUI(CompPlayer* player, bool isRight);

  // –¼‘OŽæ“¾
  const char* GetName() const override { return "MagazineUI"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

private:
  CompWeaponBase* weapon = nullptr;
  std::weak_ptr<CompSprite2D> sprite;
  bool isRight = false;

  const DirectX::XMFLOAT4 DEFAULT_COLOR = { 0.92f,1.0f,0.96f,0.84f };
  const DirectX::XMFLOAT4 RELOAD_COLOR = { 1.5f,0.0f,0.0f,0.84f };

};
