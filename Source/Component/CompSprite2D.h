#pragma once

#include "CompSpriteBase.h"

class Sprite2DRenderer;

// スプライト
class CompSprite2D : public CompSpriteBase
{
public:
  CompSprite2D(Sprite2DRenderer* renderer, const char* filename = "CreateDummy");

  // 名前取得
  virtual const char* GetName() const { return "CompSprite2D"; };

  void Start()override;

  void Update(const float& elapsedTime) override;

  void OnDestroy()override;

  // GUI描画
  void DrawImGui()override;

  void SetDrawOrder(const int& order) { drawOrder = order; }
  const int GetDrawOrder() const { return drawOrder; }

private:
  Sprite2DRenderer* renderer = nullptr;
  int drawOrder = 0;

};