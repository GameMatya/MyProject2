#pragma once

#include "GameObject.h"
#include "Component/CompCharacter.h"
#include "Component/CompPlayer.h"
#include <vector>

class CharacterManager
{
public:
  ~CharacterManager() { Clear(); }

  // �L�����N�^�[�̓o�^
  void RegisterCharacter(WeakObject character);

  // �L�����N�^�[�̓o�^����
  void RemoveCharacter(WeakObject character);

  // �v���C���[�̓o�^
  void RegisterPlayer(WeakObject player) {
    this->player = player;
  }

  // �G�̓o�^
  void RegisterEnemy(WeakObject enemy);

  // �G�̓o�^����
  void RemoveEnemy(WeakObject enemy);

  void Clear() {
    enemies.clear();
    characters.clear();
  }

  // �L�����N�^�[�Ƃ̓����蔻�� ��O������GameObject*�������Ɏ���void�֐�
  bool CharacterCollide(CompTriggerCollision* colliderA, bool isOnce, std::function<void(GameObject*)> onCollision = nullptr);

  // �L�����N�^�[�R���e�i�̎擾
  std::vector<WeakObject> GetCharacters() { return characters; }

  // �v���C���[�I�u�W�F�N�g�̎擾
  WeakObject GetPlayer() { return player; }

  // �G�I�u�W�F�N�g�R���e�i�̎擾
  std::vector<WeakObject> GetEnemies() { return enemies; }

private:
  std::vector<WeakObject> characters;
  WeakObject player;
  std::vector<WeakObject> enemies;

};
