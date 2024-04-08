#pragma once

#include "State.h"
#include "Component/ModelAnimator.h"

class CompArmoredCore;
class CompGun;

class AcArmShot :public State<CompArmoredCore>
{
public:
  AcArmShot(CompArmoredCore* owner, const ModelAnimator::ANIM_AREA& area);
  void Enter() override;
  void Execute(const float& elapsedTime) override;
  void Exit() override;

private:
  // �A�j���[�V������؂�ւ��邩�m�F
  void AnimeChangeCheck(bool& changeAnime);

  // �A�j���[�V������؂�ւ���
  void UpdateAnimeState();

private:
  const ModelAnimator::ANIM_AREA armArea;
  ModelAnimator* animator = nullptr;
  CompGun* gun = nullptr;
  bool isShot = false;

  enum ANIMATION_STATE {
    ENTER = 0,
    LOOP,
    END
  };

  int animationState = 0; // Enter -> Loop -> End �A�j���[�V�����̑J��

};