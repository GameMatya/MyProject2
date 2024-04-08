#include "CharacterManager.h"
#include "Component/CompPlayer.h"

void CharacterManager::RegisterCharacter(WeakObject character)
{
#ifdef _DEBUG
  std::vector<WeakObject>::iterator it = std::find_if(characters.begin(), characters.end(), [&](const WeakObject& ptr1) {
    return ptr1.lock() == character.lock();
    });
  assert(it == characters.end());
#endif // _DEBUG

  characters.emplace_back(character);
}

void CharacterManager::RemoveCharacter(WeakObject character)
{
  std::vector<WeakObject>::iterator it = std::find_if(characters.begin(), characters.end(), [&](const WeakObject& ptr1) {
    return ptr1.lock() == character.lock();
    });

  assert(it != characters.end());

  // �G���v���C���[������
  if ((*it).lock()->GetComponent<CompPlayer>() == nullptr) {
    // �G���X�g����폜
    RemoveEnemy(character);
  }
  else {
    // �v���C���[�폜
    player.reset();
  }

  characters.erase(it);
}

void CharacterManager::RegisterEnemy(WeakObject enemy) {
#ifdef _DEBUG
  // �����I�u�W�F�N�g�����łɓo�^����Ă��Ȃ����m�F
  std::vector<WeakObject>::iterator it = std::find_if(enemies.begin(), enemies.end(), [&](const WeakObject& ptr1) {
    return ptr1.lock() == enemy.lock();
    });
  assert(it == enemies.end());
#endif // _DEBUG

  enemies.emplace_back(enemy);
}

void CharacterManager::RemoveEnemy(WeakObject enemy)
{
  std::vector<WeakObject>::iterator it = std::find_if(enemies.begin(), enemies.end(), [&](const WeakObject& ptr1) {
    return ptr1.lock() == enemy.lock();
    });

  assert(it != enemies.end());

  enemies.erase(it);
}

bool CharacterManager::CharacterCollide(CompTriggerCollision* colliderA, bool isOnce, std::function<void(GameObject*)> onCollision)
{
  bool result = false;

  // �S�L�����N�^�[�� For���[�v
  for (int i = 0; i < characters.size(); ++i)
  {
    std::weak_ptr<CompTriggerCollision> colliderB = characters.at(i).lock()->GetComponent<CompTriggerCollision>();

    // �����蔻��R���|�[�l���g��������Ζ���
    if (colliderB.expired())continue;

    // �U���Ώۂ���Ȃ���Ζ���
    if (colliderB.lock()->GetTag() != colliderA->GetAttackTag())continue;

    // �������Ă���
    if (colliderA->Collide(colliderB.lock().get())) {
      if (onCollision != nullptr)
        onCollision(characters.at(i).lock().get());

      result = true;

      if (isOnce == true)break;
    }
  }

  return result;
}
