#include "CompStraightBullet.h"
#include "CompCharacter.h"

void CompStraightBullet::Update(const float& elapsedTime)
{
  // �ʒu�̍X�V
  UpdatePosition(elapsedTime);

  // �e���Փ˂���A�������́A�������Ԃ��s����� ���g��j��
  if (UpdateLifeTimer(elapsedTime) || CollideRivals() || CollideStage()) {
    gameObject.lock()->Destroy();
  }
}

void CompStraightBullet::OnCollision(GameObject* hitObject)
{
  CompCharacter* compChara = hitObject->GetComponent<CompCharacter>().get();

  // �_���[�W����
  compChara->ApplyDamage(attackPower, invincibleTime);
}

void CompStraightBullet::AddBulletComp(GameObject* bulletObj, const DirectX::XMFLOAT3& dir)
{
  auto comp = bulletObj->AddComponent<CompStraightBullet>(trailPath, trailRadius,
    attackPower, speed, lifeTimer, invincibleTime);
  comp->CreateTrailEffect();
  comp->SetMoveVec(dir);
}
