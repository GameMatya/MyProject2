#include "SceneResult.h"
#include "SceneSystem/SceneManager.h"
#include "SceneTitle.h"
#include "System/Input.h"

#include "Component/CompFontNumber.h"

#include <string>

void SceneResult::InitializeGameObjects()
{
  // リザルト背景
  {
    GameObject* sprTitle = objectManager.Create(TASK_LAYER::NONE).get();
    sprTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/ResultBase.png")
      ->SetTexSize({ 1920,1080 });
  }

  // タイトルボタン
  {
    GameObject* toTitle = objectManager.Create(TASK_LAYER::NONE).get();
    if (Input::Instance().GetGamePad().IsConnect() == true)
      toTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/ButtonA.png");

    else
      toTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/SpaceKey.png");
  }

  // オーディオ
  bgm = Audio::Instance().LoadAudioSource("Data/Audio/BGM/ResultBGM.wav");
  bgm->Play(true);
  bgm->setVolume(0.2f);
}

void SceneResult::Finalize()
{
  Scene::Finalize();

  bgm->Stop();
}

void SceneResult::Update(const float& elapsedTime)
{
  objectManager.Update(elapsedTime);
}

#ifdef _DEBUG
void SceneResult::ImGuiRender()
{
  objectManager.RenderGui();
}
#endif