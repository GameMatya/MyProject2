#pragma once

#include "CompSword.h"

class CompPileBunker : public CompSword
{
public:
  CompPileBunker(float interval, COLLIDER_TAGS attackTag, float damageVolume);

  // 名前取得
  const char* GetName() const override { return "PileBunker"; }

  // アニメーション再生
  void PlayAnimation()override;

};
