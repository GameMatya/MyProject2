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

  // 敵かプレイヤーか判別
  if ((*it).lock()->GetComponent<CompPlayer>() == nullptr) {
    // 敵リストから削除
    RemoveEnemy(character);
  }
  else {
    // プレイヤー削除
    player.reset();
  }

  characters.erase(it);
}

void CharacterManager::RegisterEnemy(WeakObject enemy) {
#ifdef _DEBUG
  // 同じオブジェクトがすでに登録されていないか確認
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

  // 全キャラクター分 Forループ
  for (int i = 0; i < characters.size(); ++i)
  {
    std::weak_ptr<CompTriggerCollision> colliderB = characters.at(i).lock()->GetComponent<CompTriggerCollision>();

    // 当たり判定コンポーネントが無ければ無視
    if (colliderB.expired())continue;

    // 攻撃対象じゃなければ無視
    if (colliderB.lock()->GetTag() != colliderA->GetAttackTag())continue;

    // 当たっていた
    if (colliderA->Collide(colliderB.lock().get())) {
      if (onCollision != nullptr)
        onCollision(characters.at(i).lock().get());

      result = true;

      if (isOnce == true)break;
    }
  }

  return result;
}
