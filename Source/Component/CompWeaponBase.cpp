#include "CompWeaponBase.h"
#include "CompModel.h"

void CompWeaponBase::Start()
{
  owner = gameObject.lock()->transform.GetParentObject();

  // �m�[�h���擾
  CompModel::Node* node = gameObject.lock()->GetComponent<CompModel>()->FindNode("WeaponNode");
  if (node != nullptr) {
    weaponNode = &node->worldTransform;
  }

  // �U���^�O��ݒ�
  CompTriggerCollision* compCollision = gameObject.lock()->GetComponent<CompTriggerCollision>().get();
  if (compCollision != nullptr)
    compCollision->SetAttackTag(attackTag);
}

void CompWeaponBase::Update(const float& elapsedTime)
{
  // �����e������Ȃ玩���Ń����[�h
  if (bullets == 0 && reloadTimer <= 0.0f) {
    reloadTimer = reloadTime;
  }

  if (reloadTimer > 0.0f) {
    // �����[�h����
    reloadTimer -= elapsedTime;

    if (reloadTimer <= 0.0f) {
      bullets = MAGAZINE_SIZE;
      reloadTimer = 0.0f;
    }
  }
}
