#include "SceneGame.h"
#include "SceneResult.h"
#include "SceneSystem/SceneManager.h"

#include "Camera/Camera.h"

#include "GameObject/GameObjectManager.h"

#pragma region コンポーネントのインクルード
#include "Component/CompModel.h"
#include "Component/CompCollision.h"
#include "Component/CompPlayer.h"
#include "Component/CompStage.h"
#include "Component/CompSprite3D.h"
#include "Component/CompFontNumber.h"

#include "Component/CompCameraPlayer.h"
#include "Component/CompCameraFree.h"

#include "Component/CompParticleEmitter.h"

#pragma endregion

#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"
#include "Graphics/TrailEffectManager.h"
#include "Graphics/EffectManager.h"

using namespace DirectX;

void SceneGame::InitializeGameObjects()
{
  // プレイヤーの生成
  CompPlayer* compPlayer = nullptr;
  std::weak_ptr<CompPlayer> weakPlayer;
  {
    SharedObject player = objectManager.Create(TASK_LAYER::FIRST);
    player->SetName("Player");
    player->transform.position = { 44.16f ,0.17f, -51.6f };
    player->transform.scale = { 0.002f,0.002f,0.002f };
    player->transform.rotation = { 0.0f,-0.7309f,0.0f,0.6825f };
    player->SetPushPower(10.0f);
    weakPlayer = player->AddComponent<CompPlayer>();
    compPlayer = weakPlayer.lock().get();
    CompModel* model = player->AddComponent<CompModel>("./Data/Model/HeavyRobot/HeavyRobot.model", &modelRenderer).get();
    model->AddCompCollisions(); 
  }

  // ステージ
  {
    GameObject* stage = objectManager.Create(TASK_LAYER::NONE, true).get();
    CompStaticModel* cModel = stage->AddComponent<CompStaticModel>("./Data/Model/Environment/environment.StaticModel", &modelRenderer).get();
    stage->AddComponent<CompStage>("./Data/Model/Environment/LowPolygon.StaticModel");
    stage->transform.scale = { 1.2f,1.2f,1.2f };
  }

  // 周辺減光 ( スプライト )
  {
    GameObject* vignette = objectManager.Create(TASK_LAYER::NONE).get();
    CompSprite2D* sprite = vignette->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Effect/vignette.png").get();
    sprite->SetBsMode(BS_MODE::MULTIPLY);
    sprite->SetDrawOrder(-1);
  }
}

void SceneGame::Initialize()
{
  // フォントを全て透明化
  fontRenderer.EnforceColorAlpha(0.0f);

  Graphics& graphics = Graphics::Instance();

  // カメラ初期設定
  {
    Camera& camera = Camera::Instance();

    //  一画面のアスペクト比を設定
    camera.SetPerspectiveFov(
      DirectX::XMConvertToRadians(45),
      graphics.GetScreenWidth() / static_cast<float>(graphics.GetScreenHeight()),
      0.1f,
      500.0f
    );
  }

#pragma region グラフィック関係

  // 平行光源を追加
  {
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 0.393f, 0.158f, 1));
    LightManager::Instance().Register(mainDirectionalLight);
    LightManager::Instance().SetShadowLight(mainDirectionalLight);
  }

  // ポイントライト・スポットライトの追加
  {
#pragma region ポイントライト
    {
      //for (int i = -5; i < 5; i++) {
      //  Light* light = new Light(LightType::Point);
      //  light->SetPosition(DirectX::XMFLOAT3(i % 3 * 5, 0.5f, 6 * (i % 5) + 5));
      //  light->SetRange(5);
      //  light->SetColor(DirectX::XMFLOAT4(1, i / 2, i % 5, 1));
      //  LightManager::Instane().Register(light);
      //}
    }
#pragma endregion
#pragma region スポットライト
    //{
    //  Light* light = new Light(LightType::Spot);
    //  light->SetPosition(DirectX::XMFLOAT3(0, 0.5f, -1));
    //  light->SetRange(5);
    //  light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    //  LightManager::Instane().Register(light);
    //}
#pragma endregion
  }

  // ポストプロセス
  {
    postShaders = {
      graphics.GetShader(SHADER_ID_POST::COLOR_GRADING),
      graphics.GetShader(SHADER_ID_POST::DISTANCE_FOG),
    };

    postContext.colorGradingData.saturation = 0.9f;
    postContext.distanceFogData.color = { 0.32f,0.03f,0,0.05f };
  }

#pragma endregion

  // オーディオ
  bgm = Audio::Instance().LoadAudioSource("Data/Audio/BGM/GameBGM.wav");
  bgm->Play(true);
  bgm->setVolume(0.2f);
}

void SceneGame::Finalize()
{
  Scene::Finalize();
  EffectManager::Instance().Clear();

  instance = nullptr;

  bgm->Stop();
}

void SceneGame::Update(const float& elapsedTime)
{
  // ゲームオブジェクトの更新
  objectManager.Update(elapsedTime);

  // 制限時間の更新
  UpdateTimeLimit(elapsedTime);
}

#ifdef _DEBUG
void SceneGame::ImGuiRender()
{
  objectManager.RenderGui();
  LightManager::Instance().DrawDebugPrimitive();
}

#endif // _DEBUG

bool SceneGame::UpdateStandbyTime(const float& elapsedTime)
{
  standbyTimer += elapsedTime;

  return StandbyComplete();
}

void SceneGame::UpdateTimeLimit(const float& elapsedTime)
{
  // ゲーム開始までの待機時間を満たしていなければ何もしない
  if (UpdateStandbyTime(elapsedTime) == false)return;

  gameTimer -= elapsedTime;
  gameTimer = max(gameTimer, 0.0f);

  int timer = static_cast<int>(gameTimer);
  uiMinute = static_cast<float>(timer / 60);
  uiSecond = static_cast<float>(timer % 60);
  uiMilliSecond = static_cast<float>(static_cast<int>(gameTimer * 60.0f) % 60);
}
