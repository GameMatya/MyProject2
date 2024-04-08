#pragma once

#include <thread>
#include "Scene.h"
#include "Component/CompSprite2D.h"

//���[�f�B���O�V�[��
class SceneLoading :public Scene
{
public:
  SceneLoading(Scene* nextScene) :nextScene(nextScene) {}
  ~SceneLoading() override {}

  // ������
  void InitializeGameObjects() override;
  void Initialize() override;

  // �I����
  void Finalize() override;

  // �X�V����
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // ���[�f�B���O�X���b�h
  static void LoadingThread(SceneLoading* scene);
  static void GameObjectThread(Scene* scene);

private:
  Scene* nextScene = nullptr;
  // �X���b�h
  std::thread* thread = nullptr;

  CompSprite2D* loadingIcon = nullptr;

#ifdef _DEBUG
  float loadingTimer = 0.0f;

#endif // _DEBUG

};
