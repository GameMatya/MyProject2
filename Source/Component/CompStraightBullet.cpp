#include "CompStraightBullet.h"
#include "CompCharacter.h"

void CompStraightBullet::Update(const float& elapsedTime)
{
  // 位置の更新
  UpdatePosition(elapsedTime);

  // 弾が衝突する、もしくは、生存時間が尽きれば 自身を破棄
  if (UpdateLifeTimer(elapsedTime) || CollideRivals() || CollideStage()) {
    gameObject.lock()->Destroy();
  }
}

void CompStraightBullet::OnCollision(GameObject* hitObject)
{
  CompCharacter* compChara = hitObject->GetComponent<CompCharacter>().get();

  // ダメージ処理
  compChara->ApplyDamage(attackPower, invincibleTime);
}

void CompStraightBullet::AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir)
{
  auto comp = bulletObj->AddComponent<CompStraightBullet>(trailPath, trailRadius,
    attackPower, speed, lifeTimer, invincibleTime);
  comp->CreateTrailEffect();
  comp->SetMoveVec(dir);
}
