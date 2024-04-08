#pragma once

#include <thread>
#include "Scene.h"
#include "Component/CompSprite2D.h"

//ローディングシーン
class SceneLoading :public Scene
{
public:
  SceneLoading(Scene* nextScene) :nextScene(nextScene) {}
  ~SceneLoading() override {}

  // 初期化
  void InitializeGameObjects() override;
  void Initialize() override;

  // 終了化
  void Finalize() override;

  // 更新処理
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // ローディングスレッド
  static void LoadingThread(SceneLoading* scene);
  static void GameObjectThread(Scene* scene);

private:
  Scene* nextScene = nullptr;
  // スレッド
  std::thread* thread = nullptr;

  CompSprite2D* loadingIcon = nullptr;

#ifdef _DEBUG
  float loadingTimer = 0.0f;

#endif // _DEBUG

};
