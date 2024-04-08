#include "CompModelBase.h"
#include "CompCollision.h"

void CompModelBase::AddCollisionsFromResource(ModelResourceBase* resource)
{
  const ModelResourceBase::Collision& collisionResource = resource->GetCollision();
  ObjectTransform& transform = gameObject.lock()->transform;

  // �p���s�񂩂�X�P�[���l���擾
  DirectX::XMVECTOR S, R, T;
  DirectX::XMMatrixDecompose(&S, &R, &T, transform.GetWorldTransform());
  float scale = DirectX::XMVectorGetX(S);

  // �g���K�[�R���W�����̓ǂݍ���
  if (collisionResource.triggers.empty() == false) {
    CompTriggerCollision* comp = gameObject.lock()->AddComponent<CompTriggerCollision>(collisionResource.myTag, collisionResource.targetTag).get();

    for (int i = 0; i < collisionResource.triggers.size(); ++i) {
      const ModelResourceBase::Collision::Collider& trigger = collisionResource.triggers[i];

      std::shared_ptr<Collider3D> collider = std::make_shared<Collider3D>(trigger.shape, gameObject);

      collider->position = trigger.position;
      collider->capsulePosition = trigger.capsulePosition;

      // ���a�ƍ����̂݃��[���h��Ԃɕϊ����Ă���
      {
        collider->radius = trigger.radius * scale;
        collider->height = trigger.height * scale;
      }

      // �{�[�����ݒ肳��Ă�����
      if (trigger.nodeId > 0) {
        SettingNodeCollision(collider.get(), trigger);
      }

      comp->AddCollider(collider);
    }
  }

  // �R���W�����̓ǂݍ���
  if (collisionResource.collisions.size() > 0) {
    CompCollision* comp = gameObject.lock()->AddComponent<CompCollision>().get();

    for (int i = 0; i < collisionResource.collisions.size(); ++i) {
      const ModelResourceBase::Collision::Collider& collision = collisionResource.collisions[i];

      std::shared_ptr<Collider3D> collider = std::make_shared<Collider3D>(collision.shape, gameObject);

      collider->position = collision.position;
      collider->capsulePosition = collision.capsulePosition;

      // ���a�ƍ����̂݃��[���h��Ԃɕϊ����Ă���
      {
        collider->radius = collision.radius * scale;
        collider->height = collision.height * scale;
      }

      // �{�[�����ݒ肳��Ă�����
      if (collision.nodeId > 0) {
        SettingNodeCollision(collider.get(), collision);
      }

      comp->AddCollider(collider);
    }
  }
}
