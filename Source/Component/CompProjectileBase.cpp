#include "CompProjectileBase.h"
#include "SceneSystem/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"
#include "Graphics/TrailEffectManager.h"

SharedObject CompProjectileBase::CreateBullet(GameObjectManager* manager, const COLLIDER_TAGS& attackTag, const DirectX::XMFLOAT3& dir)
{
  SharedObject bullet = nullptr;

  // �I�u�W�F�N�g����
  {
    std::unique_lock<std::mutex> lock(manager->GetMutex());
    bullet = manager->Create(TASK_LAYER::THIRD);
    bullet->SetName("Bullet");
  }

  // �e�R���|�[�l���g��ǉ�
  AddBulletComp(bullet.get(), dir);

  // �����蔻�� �ǉ�
  std::shared_ptr<Collider3D> collider = std::make_shared<Collider3D>(COLLIDER_SHAPE_3D::SPHERE, bullet);
  collider->radius = radius;
  CompTriggerCollision* collision = bullet->AddComponent<CompTriggerCollision>(COLLIDER_TAGS::BULLET, attackTag).get();
  collision->AddCollider(collider);

  return bullet;
}

void CompProjectileBase::CreateTrailEffect()
{
  std::unique_lock<std::mutex> lock(TrailEffectManager::Instance().GetMutex());
  trail[0] = std::make_unique<TrailEffect>(TRAIL_VOLUME, 0.1f, trailPath);
  trail[0]->SetVelvetyRate(1.0f);
  trail[1] = std::make_unique<TrailEffect>(TRAIL_VOLUME, 0.1f, trailPath);
  trail[1]->SetVelvetyRate(1.0f);
}

bool CompProjectileBase::UpdateLifeTimer(const float& elapsedTime)
{
  lifeTimer -= elapsedTime;
  if (lifeTimer <= 0) {
    return true;
  }
  return false;
}

void CompProjectileBase::Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed)
{
  // 1�t���[��������̉�]��
  float speed = turnSpeed * elapsedTime;

  DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&directionXZ);
  Direction = DirectX::XMVector3Normalize(Direction);
  DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&gameObject.lock()->transform.rotation);

  // ��]�s�񂩂�O�����x�N�g���𒊏o
  DirectX::XMVECTOR Front;
  {
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationQuaternion(Rotation);
    DirectX::XMVECTOR OneZ = DirectX::XMVectorSet(0, 0, 1, 0);
    Front = DirectX::XMVector3TransformNormal(OneZ, Transform);
  }

  // ��]����C�ӎ����쐬
  DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Front, Direction);
  if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
  {
    return;
  }

  // �x�N�g�����m�̊p�x�̍������߂�
  float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Front, Direction));
  // �X�s�[�h�𒲐� ( �������߂��Ȃ�قǒx���Ȃ� )
  speed = min(1.0f - dot, speed);

  // ��]����
  DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
  Rotation = DirectX::XMQuaternionMultiply(Rotation, Turn);
  DirectX::XMStoreFloat4(&gameObject.lock()->transform.rotation, Rotation);
}

void CompProjectileBase::UpdatePosition(const float& elapsedTime)
{
  ObjectTransform& transform = gameObject.lock()->transform;
  transform.position += moveDir * speed;

  if (trail[0] != nullptr) {
    DirectX::XMFLOAT3 width = Mathf::Cross(DirectX::XMFLOAT3(0, 1, 0), moveDir) * trailRadius;
    trail[0]->Set(transform.position + DirectX::XMFLOAT3(0, trailRadius, 0), transform.position - DirectX::XMFLOAT3(0, trailRadius, 0));
    trail[1]->Set(transform.position + width, transform.position - width);
  }
}

bool CompProjectileBase::CollideRivals()
{
  CompTriggerCollision* colliderA = gameObject.lock()->GetComponent<CompTriggerCollision>().get();
  std::function<void(GameObject*)> onCollision = std::bind(&CompProjectileBase::OnCollision, this, std::placeholders::_1);

  return gameObject.lock()->GetObjectManager()->GetCharacterManager().CharacterCollide(colliderA, isCollideOnce, onCollision);
}

bool CompProjectileBase::CollideStage()
{
  ObjectTransform& transform = gameObject.lock()->transform;

  HitResult result = {};
  if (gameObject.lock()->GetObjectManager()->GetStageManager().Collision(oldPosition, transform.position, &result)) {
    transform.position = result.position;

    return true;
  }

  return false;
}
