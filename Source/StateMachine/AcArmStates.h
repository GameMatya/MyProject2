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
  // アニメーションを切り替えるか確認
  void AnimeChangeCheck(bool& changeAnime);

  // アニメーションを切り替える
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

  int animationState = 0; // Enter -> Loop -> End アニメーションの遷移

};