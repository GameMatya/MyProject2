#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneSystem/SceneManager.h"

#include "System/Input.h"
#include "Camera/Camera.h"

#include "GameObject/GameObjectManager.h"
#include "Component/CompModel.h"
#include "Component/CompSprite2D.h"
#include "Component/CompParticleEmitter.h"

#include "Component/CompCameraFree.h"

#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"

using namespace DirectX;

void SceneTitle::InitializeGameObjects()
{
  // プレイヤー
  {
    SharedObject player = objectManager.Create(TASK_LAYER::FIRST);
    player->SetName("Player");
    player->transform.position = { 56.3f ,8.5f, 5.0f };
    player->transform.scale = { 0.02f,0.02f,0.02f };
    player->transform.rotation = { 0.0f,0.8f,0.0f,-0.6f };
    CompModel* model = player->AddComponent<CompModel>("./Data/Model/OvoCat/OvoCat.model", &modelRenderer).get();
    model->animator.PlayAnimation(ModelAnimator::BOTTOM, model->animator.FindAnimationId("Attack"), true, 0.0f);

    // カメラ
    {
      GameObject* obj = objectManager.Create(TASK_LAYER::SECOND).get();
      obj->SetName("Camera");
      CompCameraFree* camera = obj->AddComponent<CompCameraFree>().get();
      camera->SetTargetFocus({ 54.4f,9.75f,4.91f });
      camera->SetTargetEye({ 53.4f,9.56f,5.3f });
      camera->SetAngle({ -0.17f,-1.13f,0 });
      camera->SetDistance(1.08f);
    }
  }

  // ステージ
  {
    GameObject* stage = objectManager.Create(TASK_LAYER::NONE, true).get();
    CompStaticModel* cModel = stage->AddComponent<CompStaticModel>("./Data/Model/Environment/SnowMountain.StaticModel", &modelRenderer).get();
    stage->transform.scale = { 0.4f,0.4f,0.4f };
  }
  // ステージエフェクト
  {
    GameObject* effect = objectManager.Create(TASK_LAYER::FIRST).get();
    effect->SetName("StageParticle");
    effect->transform.position = { 53.4f,9.56f,5.3f };
    CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(1500, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
    comp->SetEmitRate(1);
    comp->SetDuration(0.0f);
    comp->SetEmitInterval(0.02f);
    comp->emitterData.lifeTime = 1.0f;
    comp->emitterData.emissivePower = 10.0f;
    comp->emitterData.spawnRandScale = 60.0f;
    comp->emitterData.vecRandScale = 1.0f;
    comp->emitterData.startSize = 0.12f;
    comp->emitterData.endSize = 0.004f;
    comp->emitterData.particleSpeed = 0.7f;
    comp->emitterData.speedRandScale = 0.5f;
    comp->emitterData.startSpeedRate = 1.0f;
    comp->emitterData.endSpeedRate = 0.2f;
    comp->emitterData.startColor = { 0.0f,0.01f,1.0f,1.0f };
    comp->emitterData.endColor = { 0.0f,0.005f,1.0f,1.0f };

#pragma endregion
  }
}

void SceneTitle::Initialize()
{
  // カメラ初期設定
  {
    Graphics& graphics = Graphics::Instance();
    Camera& camera = Camera::Instance();

    //  一画面のアスペクト比を設定
    camera.SetPerspectiveFov(
      DirectX::XMConvertToRadians(45),
      graphics.GetScreenWidth() / static_cast<float>(graphics.GetScreenHeight()),
      0.1f,
      1500.0f
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

  // ポストプロセス
  {
    Graphics& graphics = Graphics::Instance();
    postShaders = {
      graphics.GetShader(SHADER_ID_POST::COLOR_GRADING),
    };

    postContext.colorGradingData.saturation = 0.9f;
    postContext.bloomData.threshold = 1.4f;
  }

#pragma endregion

}

void SceneTitle::Finalize()
{
  Scene::Finalize();
}

void SceneTitle::Update(const float& elapsedTime)
{
  objectManager.Update(elapsedTime);

  SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
}

#ifdef _DEBUG
void SceneTitle::ImGuiRender()
{
  objectManager.RenderGui();
  LightManager::Instance().DrawDebugPrimitive();
}

#endif // _DEBUG
