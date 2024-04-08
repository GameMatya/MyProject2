#include "Graphics/Graphics.h"
#include "SceneLoading.h"
#include "SceneManager.h"

void SceneLoading::InitializeGameObjects()
{
}

// ������
void SceneLoading::Initialize()
{
  // ���[�f�B���O�w�i
  {
    SharedObject obj = objectManager.Create(TASK_LAYER::FIRST);
    CompSprite2D* sprite = obj->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/loading.png").get();
  }
  //// ���[�f�B���O�A�C�R��
  //{
  //  SharedObject obj = objectManager.Create(TASK_LAYER::FIRST);
  //  loadingIcon = obj->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/loadingIcon.png").get();
  //  obj->transform.position = { 1200.0f,1050.0f,0.0f };
  //  loadingIcon->SetPivot(loadingIcon->GetTexSize() / 2.0f);
  //  loadingIcon->SetColor({ 1.0f,1.0f,1.0f,0.75f });
  //}

  // �X���b�h�J�n
  thread = new std::thread(LoadingThread, this);

  //loadingIcon = std::make_unique<Sprite>();

}

// �I����
void SceneLoading::Finalize()
{
  // �X���b�h�I����
  if (thread != nullptr)
  {
    thread->join();
    delete thread;
    thread = nullptr;
  }

}

// �X�V����
void SceneLoading::Update(const float& elapsedTime)
{
  objectManager.Update(elapsedTime);
  //loadingIcon->SetAngle(loadingIcon->GetAngle() + elapsedTime);

  // ���̃V�[���̏���������������V�[����؂�ւ���
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

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
  // COM�֘A�̏�����
  CoInitialize(nullptr);

  // ModelRenderer�̏������̃X���b�h�J�n
  std::thread* modelRendererThread = new std::thread(GameObjectThread, scene->nextScene);

  // ���̃V�[���̏��������s��
  scene->nextScene->Initialize();

  // ModelRenderer�̏�������҂�
  modelRendererThread->join();
  delete modelRendererThread;

  // �X���b�h���I���O��COM�֘A�̏I����
  CoUninitialize();

  // ���̃V�[���̏��������ݒ�
  scene->nextScene->SetReady();
}

void SceneLoading::GameObjectThread(Scene* scene)
{
  // COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
  CoInitialize(nullptr);

  // ���̃V�[����GameObject�̏��������s��
  scene->InitializeGameObjects();

  scene->GetGameObjectManager().Initialize();

  // �X���b�h���I���O��COM�֘A�̏I����
  CoUninitialize();
}
