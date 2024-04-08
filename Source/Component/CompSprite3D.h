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

  // ���O�擾
  virtual const char* GetName() const { return "CompSprite3D"; };

  void Start() override;

  void Update(const float& elapsedTime);

  // �r���{�[�h������K�p���鎲��ݒ�
  void SetBillboardData(const BILLBOARD_AXIS& axis) { billboardData = axis; }
  BILLBOARD_AXIS GetBillboardData() { return billboardData; }

private:
  Sprite3DRenderer* renderer = nullptr;

  // �r���{�[�h�K�p���
  BILLBOARD_AXIS billboardData = BILLBOARD_AXIS::NONE;

};
