#pragma once

#include "CompSword.h"

class CompPileBunker : public CompSword
{
public:
  CompPileBunker(float interval, COLLIDER_TAGS attackTag, float damageVolume);

  // ���O�擾
  const char* GetName() const override { return "PileBunker"; }

  // �A�j���[�V�����Đ�
  void PlayAnimation()override;

};
