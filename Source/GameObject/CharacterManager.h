#pragma once

#include "GameObject.h"
#include "Component/CompCharacter.h"
#include "Component/CompPlayer.h"
#include <vector>

class CharacterManager
{
public:
  ~CharacterManager() { Clear(); }

  // キャラクターの登録
  void RegisterCharacter(WeakObject character);

  // キャラクターの登録解除
  void RemoveCharacter(WeakObject character);

  // プレイヤーの登録
  void RegisterPlayer(WeakObject player) {
    this->player = player;
  }

  // 敵の登録
  void RegisterEnemy(WeakObject enemy);

  // 敵の登録解除
  void RemoveEnemy(WeakObject enemy);

  void Clear() {
    enemies.clear();
    characters.clear();
  }

  // キャラクターとの当たり判定 第三引数はGameObject*を引数に持つvoid関数
  bool CharacterCollide(CompTriggerCollision* colliderA, bool isOnce, std::function<void(GameObject*)> onCollision = nullptr);

  // キャラクターコンテナの取得
  std::vector<WeakObject> GetCharacters() { return characters; }

  // プレイヤーオブジェクトの取得
  WeakObject GetPlayer() { return player; }

  // 敵オブジェクトコンテナの取得
  std::vector<WeakObject> GetEnemies() { return enemies; }

private:
  std::vector<WeakObject> characters;
  WeakObject player;
  std::vector<WeakObject> enemies;

};
