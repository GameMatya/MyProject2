#include "CompCollision.h"
#include "Graphics/Graphics.h"

bool CompCollision::Collide(CompCollision* other)
{
  // ２つの球の重さから押し出し比率を求める
  float rate = gameObject.lock()->GetPushPower() / (gameObject.lock()->GetPushPower() + other->gameObject.lock()->GetPushPower());

  // 全ての当たり判定と判定を取る
  for (int i = 0; i < colliders.size(); ++i) {
    for (int j = 0; j < other->GetColliders().size(); ++j) {
      colliders.at(i)->Collision(*other->GetColliders().at(j), false, rate);
    }
  }

  return false;
}

void CompCollision::DrawImGui()
{
  for (int i = 0; i < colliders.size(); ++i) {
    colliders.at(i)->DrawDebug({ 1,1,0,1 });
  }
}
