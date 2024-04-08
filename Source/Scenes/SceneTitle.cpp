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
    player->transform.scale = { 0.002f,0.002f,0.002f };
    player->transform.rotation = { 0.0f,0.8f,0.0f,-0.6f };
    CompModel* model = player->AddComponent<CompModel>("./Data/Model/HeavyRobot/HeavyRobot.model", &modelRenderer).get();
    model->animator.PlayAnimation(ModelAnimator::BOTTOM, model->animator.FindAnimationId("QuickBoost_Forward"), true, 0.0f);

    // プレイヤーの右手武装
    {
      GameObject* obj = objectManager.Create(TASK_LAYER::SECOND).get();
      obj->SetName("PlayerRightWeapon");
      obj->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("RightWeapon")->worldTransform);
      obj->AddComponent<CompModel>("./Data/Model/RailGun/RailGun.model", &modelRenderer);
    }
    // プレイヤーの左手武装
    {
      GameObject* obj = objectManager.Create(TASK_LAYER::SECOND).get();
      obj->SetName("PlayerLeftWeapon");
      obj->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LeftWeapon")->worldTransform);
      obj->AddComponent<CompModel>("./Data/Model/Sword/Sword.model", &modelRenderer)->AddCompCollisions();
    }

#if(1) // 機体のパーティクルエフェクト
    // エフェクト( 後ろバーニア上 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("BackThrusterUP");
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(4000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Ob_U")->worldTransform);

#pragma region パラメーター
      effect->transform.position = { 0.0f,130.0f,0.0f };
      effect->transform.rotation = { 0.0f,2.5f,-0.872f,0.0f };

      comp->SetIsLoop(true);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(12);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.38f;
      comp->emitterData.emissivePower = 3.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.0012f;
      comp->emitterData.startSize = 0.02f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = 2.2f;
      comp->emitterData.speedRandScale = 8.0f;
      comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
      comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
    }

    // エフェクト( 後ろバーニア下 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("BackThrusterDOWN");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Ob_L")->worldTransform);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(4000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,100.0f,0.0f };
      effect->transform.rotation = { 0.0f,1.2f,-0.955f,0.0f };

      comp->SetIsLoop(true);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(14);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.3f;
      comp->emitterData.emissivePower = 4.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.001f;
      comp->emitterData.startSize = 0.02f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = 0.6f;
      comp->emitterData.speedRandScale = 8.0f;
      comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
      comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
    }

    // エフェクト( 左足 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("LeftLeg");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_L")->worldTransform);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,-130.0f,0.0f };
      effect->transform.rotation = { -0.5f,-0.3f,0.0f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(15);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.12f;
      comp->emitterData.emissivePower = 6.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.009f;
      comp->emitterData.startSize = 0.018f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = -1.0f;
      comp->emitterData.speedRandScale = -4.0f;
      comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
      comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
    }

    // エフェクト( 右足 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("RightLeg");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_R")->worldTransform);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,-130.0f,0.0f };
      effect->transform.rotation = { 0.5f,-0.5f,0.06f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(10);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.25f;
      comp->emitterData.emissivePower = 6.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.0003f;
      comp->emitterData.startSize = 0.018f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = 3.0f;
      comp->emitterData.speedRandScale = 1.2f;
      comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
      comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
    }

    // 地面移動時のエフェクト ( 砂煙 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND, true).get();
      effect->SetName("LandDustParticle");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Rb_Toe_R")->worldTransform);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(5000, "./Data/Sprite/Effect/landParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,70.0f,-70.0f };
      effect->transform.rotation = { 0.2f,-0.4f,1.0f,0.0f };

      comp->SetBsMode(BS_MODE::ALPHA);
      comp->SetIsLoop(true);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(9);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.3f;
      comp->emitterData.emissivePower = 0.0f;
      comp->emitterData.spawnRandScale = 0.6f;
      comp->emitterData.vecRandScale = 0.2f;
      comp->emitterData.startSize = 0.2f;
      comp->emitterData.endSize = 0.5f;
      comp->emitterData.particleSpeed = 1.8f;
      comp->emitterData.speedRandScale = 1.5f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 0.2f;
      comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.1f };
      comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
    }
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND, true).get();
      effect->SetName("LandDustParticle");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Rb_Toe_L")->worldTransform);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(5000, "./Data/Sprite/Effect/landParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,70.0f,-70.0f };
      effect->transform.rotation = { 1.0f,0.0f,0.0f,0.0f };

      comp->SetBsMode(BS_MODE::ALPHA);
      comp->SetIsLoop(true);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(9);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.3f;
      comp->emitterData.emissivePower = 0.0f;
      comp->emitterData.spawnRandScale = 0.6f;
      comp->emitterData.vecRandScale = 0.2f;
      comp->emitterData.startSize = 0.2f;
      comp->emitterData.endSize = 0.5f;
      comp->emitterData.particleSpeed = 1.8f;
      comp->emitterData.speedRandScale = 1.5f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 0.2f;
      comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.1f };
      comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
    }
#endif

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
    CompStaticModel* cModel = stage->AddComponent<CompStaticModel>("./Data/Model/Environment/environment.StaticModel", &modelRenderer).get();
    stage->transform.scale = { 1.5f,1.5f,1.5f };
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

  // ポストプロセス
  {
    Graphics& graphics = Graphics::Instance();
    postShaders = {
      graphics.GetShader(SHADER_ID_POST::COLOR_GRADING),
    };

    postContext.colorGradingData.saturation = 0.9f;
  }

#pragma endregion

  // オーディオ
  bgm = Audio::Instance().LoadAudioSource("Data/Audio/BGM/TitleBGM.wav");
  bgm->Play(true);
  bgm->setVolume(0.2f);
}

void SceneTitle::Finalize()
{
  Scene::Finalize();

  bgm->Stop();
}

void SceneTitle::Update(const float& elapsedTime)
{
  objectManager.Update(elapsedTime);
}

#ifdef _DEBUG
void SceneTitle::ImGuiRender()
{
  objectManager.RenderGui();
  LightManager::Instance().DrawDebugPrimitive();
}

#endif // _DEBUG
