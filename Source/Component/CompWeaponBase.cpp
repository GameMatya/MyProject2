#include "CompWeaponBase.h"
#include "CompModel.h"

void CompWeaponBase::Start()
{
  owner = gameObject.lock()->transform.GetParentObject();

  // ノードを取得
  CompModel::Node* node = gameObject.lock()->GetComponent<CompModel>()->FindNode("WeaponNode");
  if (node != nullptr) {
    weaponNode = &node->worldTransform;
  }

  // 攻撃タグを設定
  CompTriggerCollision* compCollision = gameObject.lock()->GetComponent<CompTriggerCollision>().get();
  if (compCollision != nullptr)
    compCollision->SetAttackTag(attackTag);
}

void CompWeaponBase::Update(const float& elapsedTime)
{
  // 所持弾数が空なら自動でリロード
  if (bullets == 0 && reloadTimer <= 0.0f) {
    reloadTimer = reloadTime;
  }

  if (reloadTimer > 0.0f) {
    // リロード処理
    reloadTimer -= elapsedTime;

    if (reloadTimer <= 0.0f) {
      bullets = MAGAZINE_SIZE;
      reloadTimer = 0.0f;
    }
  }
}
