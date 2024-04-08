#include "SceneManager.h"
#include "SceneRenderer.h"

// 更新処理
void SceneManager::Update(const float& elapsedTime)
{
  if (nextScene != nullptr)
  {
    // 古いシーンを終了処理
    Clear();

    // 新しいシーンを設定
    currentScene = nextScene;

    // シーン初期化処理
    if (!currentScene->IsReady())
    {
      currentScene->Initialize();
    }
    nextScene = nullptr;
  }

  if (currentScene != nullptr)
  {
    currentScene->Update(elapsedTime);
    SceneRenderer::Instance().SetElapsedTime(elapsedTime);
  }
}

// 描画処理
void SceneManager::Render()
{
  if (currentScene != nullptr)
  {
    currentScene->Render();
  }
}

// シーンクリア
void SceneManager::Clear()
{
  if (currentScene != nullptr)
  {
    currentScene->Finalize();
    delete currentScene;
    currentScene = nullptr;
  }
}

// シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
  // 新しいシーンを設定
  nextScene = scene;
}