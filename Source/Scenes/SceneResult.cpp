#include "SceneResult.h"
#include "SceneSystem/SceneManager.h"
#include "SceneTitle.h"
#include "System/Input.h"

#include "GameSystem/ScoreManager.h"

#include "Component/CompFontNumber.h"
#include "Component/CompBooleanSprite.h"

#include <string>

void SceneResult::InitializeGameObjects()
{
  // ���U���g�w�i
  {
    GameObject* sprTitle = objectManager.Create(TASK_LAYER::NONE).get();
    sprTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/ResultBase.png")
      ->SetTexSize({ 1920,1080 });
  }

  // ���j��
  {
    GameObject* count = objectManager.Create(TASK_LAYER::FIRST).get();
    count->transform.position = { 450.0f,214.0f,0.0f };
    count->transform.scale = { 0.4f,0.4f,0.0f };
    CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
    count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetKillCount(), 0, 2);
    font->SetColor({ 0.92f,1.0f,0.96f, 1.0f });
  }
  // ���j�X�R�A
  {
    GameObject* count = objectManager.Create(TASK_LAYER::FIRST).get();
    count->transform.position = { 810.0f,214.0f,0.0f };
    count->transform.scale = { 0.4f,0.4f,0.0f };
    CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 5).get();
    count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetScorePoint(), 0, 5);
    font->SetColor({ 0.92f,1.0f,0.96f, 1.0f });
  }

  // �̗͊���
  {
    GameObject* count = objectManager.Create(TASK_LAYER::FIRST).get();
    count->transform.position = { 419.0f,269.0f,0.0f };
    count->transform.scale = { 0.4f,0.4f,0.0f };
    CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 3).get();
    count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetHealthRate(), 0, 3);
    font->SetColor({ 0.92f,1.0f,0.96f, 1.0f });
  }
  // �̗̓X�R�A
  {
    GameObject* count = objectManager.Create(TASK_LAYER::FIRST).get();
    count->transform.position = { 810.0f,269.0f,0.0f };
    count->transform.scale = { 0.4f,0.4f,0.0f };
    CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 5).get();
    count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetHealthScore(), 0, 5);
    font->SetColor({ 1.0f,0.0f,0.0f, 1.0f });
  }

  // �ŏI�X�R�A
  {
    GameObject* count = objectManager.Create(TASK_LAYER::FIRST).get();
    count->transform.position = { 809.0f,328.0f,0.0f };
    count->transform.scale = { 0.6f,0.6f,0.0f };
    CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 5).get();
    count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetFinalScore(), 0, 5);
    font->SetColor({ 0.92f,1.0f,0.96f,1.0f });
  }

  // �X�R�A�����N
  {
    GameObject* rank = objectManager.Create(TASK_LAYER::NONE).get();
    int score = static_cast<int>(ScoreManager::Instance().GetScoreRank());

    std::string texturePath = "./Data/Sprite/Text/Rank_";
    texturePath += std::to_string(score);
    texturePath += ".png";

    rank->AddComponent<CompSprite2D>(&sprite2DRenderer, texturePath.c_str());
  }

  // �^�C�g���{�^��
  {
    GameObject* toTitle = objectManager.Create(TASK_LAYER::NONE).get();
    if (Input::Instance().GetGamePad().IsConnect() == true)
      toTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/ButtonA.png");

    else
      toTitle->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/SpaceKey.png");
  }

  // �I�[�f�B�I
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

  fixedTimer += elapsedTime * (GetFadeOutFlag() ? -1.0f : 1.0f);

  // �^�C�g���V�[����
  if (GetFadeOutFlag() == false) {
    if (Input::Instance().GetGamePad().GetButtonDown(GamePad::BTN_A)) {
      SwitchFade();
    }
  }

  // �t�F�[�h�A�E�g������������V�[���J��
  if (fadeSprite.lock()->GetIsDisplay() == true) {
    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
  }
}

#ifdef _DEBUG
void SceneResult::ImGuiRender()
{
  objectManager.RenderGui();
}
#endif