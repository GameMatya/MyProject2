#include "CompSword.h"
#include "CompTriggerCollision.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"
#include "TimeManager/TimeManager.h"

CompSword::CompSword(float effectRange, float interval, COLLIDER_TAGS attackTag, float damageVolume, const char* trailFilepath)
  :effectRange(effectRange), attackPower(damageVolume), CompWeaponBase(1, interval, 1.0f, attackTag)
{
  if (trailFilepath != nullptr) {
    trailEffect = std::make_unique<TrailEffect>(TRAIL_VOLUME, 0.3f, trailFilepath);
  }

  hitSE = Audio::Instance().LoadAudioSource("./Data/Audio/SE/HitSlash.wav");
}

void CompSword::Update(const float& elapsedTime)
{
  CompWeaponBase::Update(elapsedTime);

  CompTriggerCollision* triggerComp = gameObject.lock()->GetComponent<CompTriggerCollision>().get();
  gameObject.lock()->GetObjectManager()->GetCharacterManager().CharacterCollide(triggerComp, false);
}

void CompSword::UpdateEffect()
{
  // �g���C���G�t�F�N�g 
  if (trailEffect != nullptr)
  {
    DirectX::XMFLOAT4X4 baseNode;
    DirectX::XMStoreFloat4x4(&baseNode, *weaponNode);

    // �g���C���̍��� ( WeaponNode�̈ʒu )
    DirectX::XMFLOAT3 tail = { baseNode._41,baseNode._42,baseNode._43 };

    // �g���C���̐� ( WeaponNode�̈ʒu + WeaponNode�̑O���� )
    DirectX::XMFLOAT3 head = { baseNode._31,baseNode._32,baseNode._33 };
    head = Mathf::Normalize(head);
    // TODO : �g���C���̕� ( �ꎞ�I�Ȓl�A�����I�ɂ͕ϐ������G�f�B�^�[�ŕҏW�o����悤�ɂ��� )
    head *= 2;
    head += tail;

    trailEffect->Set(head, tail);
  }
}

void CompSword::Execute(const GameObject* target)
{
  // �G�Ɠ����蔻��
  {
    CompTriggerCollision* colliderA = gameObject.lock()->GetComponent<CompTriggerCollision>().get();
    std::function<void(GameObject*)> onCollision = std::bind(&CompSword::OnCollision, this, std::placeholders::_1);

    gameObject.lock()->GetObjectManager()->GetCharacterManager().CharacterCollide(
      colliderA, true, onCollision);
  }
}

void CompSword::OnCollision(GameObject* hitObject)
{
  CompCharacter* compChara = hitObject->GetComponent<CompCharacter>().get();

  // �_���[�W����
  if (compChara->ApplyDamage(attackPower, invincibleTime) == true) {
    // SE�Đ�
    hitSE->Play(false);

    // �q�b�g�X�g�b�v
    TimeManager::Instance().SetTimeEffect(hitStopScale, hitStopTime);

    // ������������𐁂���΂�
    DirectX::XMFLOAT3 impact = hitObject->transform.position - gameObject.lock()->transform.GetParentObject().lock()->transform.position;
    impact = Mathf::Normalize(impact);
    compChara->AddImpulse(impact);
  }
}
