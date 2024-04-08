#include "CompCollision.h"
#include "Graphics/Graphics.h"

bool CompCollision::Collide(CompCollision* other)
{
  // �Q�̋��̏d�����牟���o���䗦�����߂�
  float rate = gameObject.lock()->GetPushPower() / (gameObject.lock()->GetPushPower() + other->gameObject.lock()->GetPushPower());

  // �S�Ă̓����蔻��Ɣ�������
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
