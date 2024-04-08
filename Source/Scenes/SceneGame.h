#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"
#include "GameSystem/EnemySpawner.h"
#include "Audio/Audio.h"

// �^�C�g���V�[��
class SceneGame :public Scene
{
public:
  SceneGame() { instance = this; }
  ~SceneGame() override {}

  static const SceneGame* Instance() { return instance; }

  // ������
  void InitializeGameObjects() override;
  void Initialize() override;

  // �I����
  void Finalize() override;

  // �X�V����
  void Update(const float& elapsedTime) override;

  const bool StandbyComplete() const { return (standbyTimer > TIME_STANDBY); }

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // �ҋ@���Ԃ̍X�V
  bool UpdateStandbyTime(const float& elapsedTime);

  // �������Ԃ̍X�V
  void UpdateTimeLimit(const float& elapsedTime);

private:
  inline static SceneGame* instance;

  // �Q�[���J�n�܂ł̑ҋ@����
  const float TIME_STANDBY = 4.2f;
  float standbyTimer = 0.0f;

  // �Q�[���̐������ԁ{�i�s����
  const float TIME_LIMIT = 70.0f;
  float gameTimer = TIME_LIMIT;

  // �������Ԃ�UI
  float uiMinute = 0.0f;
  float uiSecond = 0.0f;
  float uiMilliSecond = 0.0f;

  //�I�[�f�B�I
  std::unique_ptr<AudioSource> bgm = nullptr;

};