#pragma once

#include <memory>
#include <set>
#include "GameObject.h"
#include "Task/TaskManager.h"
#include "StageManager.h"
#include "CharacterManager.h"

class Scene;

// �A�N�^�[�}�l�[�W���[
class GameObjectManager
{
public:
  GameObjectManager(Scene* scene) :scene(scene) {}

  // �쐬
  SharedObject Create(TASK_LAYER layer, bool isStage = false);

  // ������
  void Initialize();

  void Finalize();

  // �폜
  void Remove(SharedObject player);

  // �X�V
  void Update(const float& elapsedTime);

  // �`��
  void RenderGui();

  Scene* GetScene() { return scene; }

  // �X�e�[�W�}�l�[�W���[�̎擾
  StageManager& GetStageManager() { return stageManager; }

  // �L�����N�^�[�}�l�[�W���[�̎擾
  CharacterManager& GetCharacterManager() { return characterManager; }

  // �R���|�[�l���g�̍X�V�������ŃI�u�W�F�N�g�̐������s���p
  std::mutex& GetMutex() { return mutex; }

private:
  void Collisions();
  void DiscardObjects();

  void DrawLister();
  void DrawDetail();

private:
  Scene* scene;
  std::mutex mutex;

  // �����\��̓��I�I�u�W�F�N�g�̔z��
  std::vector<SharedObject>		startObjects;
  // ���I�I�u�W�F�N�g�̔z��
  std::vector<SharedObject>		objects;
  // �ÓI�I�u�W�F�N�g�̔z��
  std::vector<SharedObject>		staticObjects;
  // �폜�\��̃I�u�W�F�N�g�̔z��
  std::set<SharedObject>			removeObjects;

  // ���񏈗��^�X�N�̊Ǘ�
  TaskManager       taskManager;
  // �L�����N�^�[�I�u�W�F�N�g�̊Ǘ�
  CharacterManager  characterManager;
  // �X�e�[�W�I�u�W�F�N�g�̊Ǘ�
  StageManager      stageManager;

  // �f�o�b�O�p�̃I�u�W�F�N�g�|�C���^
  std::set<SharedObject>				selectionObjects;
  bool				hiddenLister = false;
  bool				hiddenDetail = false;

};
