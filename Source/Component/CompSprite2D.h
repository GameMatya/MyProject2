#pragma once

#include "CompSpriteBase.h"

class Sprite2DRenderer;

// �X�v���C�g
class CompSprite2D : public CompSpriteBase
{
public:
  CompSprite2D(Sprite2DRenderer* renderer, const char* filename = "CreateDummy");

  // ���O�擾
  virtual const char* GetName() const { return "CompSprite2D"; };

  void Start()override;

  void Update(const float& elapsedTime) override;

  void OnDestroy()override;

  // GUI�`��
  void DrawImGui()override;

  void SetDrawOrder(const int& order) { drawOrder = order; }
  const int GetDrawOrder() const { return drawOrder; }

private:
  Sprite2DRenderer* renderer = nullptr;
  int drawOrder = 0;

};