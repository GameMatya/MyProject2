#include "Graphics/Graphics.h"
#include "SceneLoading.h"
#include "SceneManager.h"

void SceneLoading::InitializeGameObjects()
{
}

// 初期化
void SceneLoading::Initialize()
{
  // ローディング背景
  {
    SharedObject obj = objectManager.Create(TASK_LAYER::FIRST);
    CompSprite2D* sprite = obj->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/loading.png").get();
  }
  //// ローディングアイコン
  //{
  //  SharedObject obj = objectManager.Create(TASK_LAYER::FIRST);
  //  loadingIcon = obj->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/loadingIcon.png").get();
  //  obj->transform.position = { 1200.0f,1050.0f,0.0f };
  //  loadingIcon->SetPivot(loadingIcon->GetTexSize() / 2.0f);
  //  loadingIcon->SetColor({ 1.0f,1.0f,1.0f,0.75f });
  //}

  // スレッド開始
  thread = new std::thread(LoadingThread, this);

  //loadingIcon = std::make_unique<Sprite>();

}

// 終了化
void SceneLoading::Finalize()
{
  // スレッド終了化
  if (thread != nullptr)
  {
    thread->join();
    delete thread;
    thread = nullptr;
  }

}

// 更新処理
void SceneLoading::Update(const float& elapsedTime)
{
  objectManager.Update(elapsedTime);
  //loadingIcon->SetAngle(loadingIcon->GetAngle() + elapsedTime);

  // 次のシーンの準備が完了したらシーンを切り替える
  if (nextScene->IsReady())
  {
    SceneManager::Instance().ChangeScene(nextScene);
    nextScene = nullptr;
  }

#ifdef _DEBUG
  loadingTimer += elapsedTime;

#endif
}

#ifdef _DEBUG
void SceneLoading::ImGuiRender()
{
  ImGui::Begin("Scene Loading");
  ImGui::InputFloat("Loading Time", &loadingTimer);
  ImGui::End();
}

#endif // _DEBUG

// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
  // COM関連の初期化
  CoInitialize(nullptr);

  // ModelRendererの初期化のスレッド開始
  std::thread* modelRendererThread = new std::thread(GameObjectThread, scene->nextScene);

  // 次のシーンの初期化を行う
  scene->nextScene->Initialize();

  // ModelRendererの初期化を待つ
  modelRendererThread->join();
  delete modelRendererThread;

  // スレッドが終わる前にCOM関連の終了化
  CoUninitialize();

  // 次のシーンの準備完了設定
  scene->nextScene->SetReady();
}

void SceneLoading::GameObjectThread(Scene* scene)
{
  // COM関連の初期化でスレッド毎に呼ぶ必要がある
  CoInitialize(nullptr);

  // 次のシーンのGameObjectの初期化を行う
  scene->InitializeGameObjects();

  scene->GetGameObjectManager().Initialize();

  // スレッドが終わる前にCOM関連の終了化
  CoUninitialize();
}
