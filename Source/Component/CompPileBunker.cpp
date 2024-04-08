#include "CompPileBunker.h"

CompPileBunker::CompPileBunker(float interval, COLLIDER_TAGS attackTag, float damageVolume)
  :CompSword(0.0f, interval, attackTag, damageVolume, nullptr)
{
  hitStopScale = 0.2f;
  hitStopTime = 0.3f;
}

void CompPileBunker::PlayAnimation()
{
  // パイルバンカー射出アニメーション再生
  std::weak_ptr<CompModel> model = gameObject.lock()->GetComponent<CompModel>();

  ModelAnimator& animator = model.lock()->animator;
  animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, 0, false);
}
