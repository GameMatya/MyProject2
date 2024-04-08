#pragma once

#include "CompSpriteBase.h"

class Sprite3DRenderer;

enum class BILLBOARD_AXIS {
  NONE,
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
  AXIS_ALL,

  MAX
};

class CompSprite3D :public CompSpriteBase
{
public:
  CompSprite3D(const char* filepath, Sprite3DRenderer* renderer);

  // 名前取得
  virtual const char* GetName() const { return "CompSprite3D"; };

  void Start() override;

  void Update(const float& elapsedTime);

  // ビルボード処理を適用する軸を設定
  void SetBillboardData(const BILLBOARD_AXIS& axis) { billboardData = axis; }
  BILLBOARD_AXIS GetBillboardData() { return billboardData; }

private:
  Sprite3DRenderer* renderer = nullptr;

  // ビルボード適用情報
  BILLBOARD_AXIS billboardData = BILLBOARD_AXIS::NONE;

};
