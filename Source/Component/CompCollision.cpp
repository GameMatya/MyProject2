#include "CompCollision.h"
#include "Graphics/Graphics.h"

bool CompCollision::Collide(CompCollision* other)
{
  // ‚Q‚Â‚Ì‹…‚Ìd‚³‚©‚ç‰Ÿ‚µo‚µ”ä—¦‚ð‹‚ß‚é
  float rate = gameObject.lock()->GetPushPower() / (gameObject.lock()->GetPushPower() + other->gameObject.lock()->GetPushPower());

  // ‘S‚Ä‚Ì“–‚½‚è”»’è‚Æ”»’è‚ðŽæ‚é
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
