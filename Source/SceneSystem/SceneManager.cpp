#include "SceneManager.h"
#include "SceneRenderer.h"

// �X�V����
void SceneManager::Update(const float& elapsedTime)
{
  if (nextScene != nullptr)
  {
    // �Â��V�[�����I������
    Clear();

    // �V�����V�[����ݒ�
    currentScene = nextScene;

    // �V�[������������
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

// �`�揈��
void SceneManager::Render()
{
  if (currentScene != nullptr)
  {
    currentScene->Render();
  }
}

// �V�[���N���A
void SceneManager::Clear()
{
  if (currentScene != nullptr)
  {
    currentScene->Finalize();
    delete currentScene;
    currentScene = nullptr;
  }
}

// �V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
  // �V�����V�[����ݒ�
  nextScene = scene;
}