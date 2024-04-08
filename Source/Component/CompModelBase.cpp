#include "CompModelBase.h"
#include "CompCollision.h"

void CompModelBase::AddCollisionsFromResource(ModelResourceBase* resource)
{
  const ModelResourceBase::Collision& collisionResource = resource->GetCollision();
  ObjectTransform& transform = gameObject.lock()->transform;

  // 姿勢行列からスケール値を取得
  DirectX::XMVECTOR S, R, T;
  DirectX::XMMatrixDecompose(&S, &R, &T, transform.GetWorldTransform());
  float scale = DirectX::XMVectorGetX(S);

  // トリガーコリジョンの読み込み
  if (collisionResource.triggers.empty() == false) {
    CompTriggerCollision* comp = gameObject.lock()->AddComponent<CompTriggerCollision>(collisionResource.myTag, collisionResource.targetTag).get();

    for (int i = 0; i < collisionResource.triggers.size(); ++i) {
      const ModelResourceBase::Collision::Collider& trigger = collisionResource.triggers[i];

      std::shared_ptr<Collider3D> collider = std::make_shared<Collider3D>(trigger.shape, gameObject);

      collider->position = trigger.position;
      collider->capsulePosition = trigger.capsulePosition;

      // 半径と高さのみワールド空間に変換しておく
      {
        collider->radius = trigger.radius * scale;
        collider->height = trigger.height * scale;
      }

      // ボーンが設定されていたら
      if (trigger.nodeId > 0) {
        SettingNodeCollision(collider.get(), trigger);
      }

      comp->AddCollider(collider);
    }
  }

  // コリジョンの読み込み
  if (collisionResource.collisions.size() > 0) {
    CompCollision* comp = gameObject.lock()->AddComponent<CompCollision>().get();

    for (int i = 0; i < collisionResource.collisions.size(); ++i) {
      const ModelResourceBase::Collision::Collider& collision = collisionResource.collisions[i];

      std::shared_ptr<Collider3D> collider = std::make_shared<Collider3D>(collision.shape, gameObject);

      collider->position = collision.position;
      collider->capsulePosition = collision.capsulePosition;

      // 半径と高さのみワールド空間に変換しておく
      {
        collider->radius = collision.radius * scale;
        collider->height = collision.height * scale;
      }

      // ボーンが設定されていたら
      if (collision.nodeId > 0) {
        SettingNodeCollision(collider.get(), collision);
      }

      comp->AddCollider(collider);
    }
  }
}
