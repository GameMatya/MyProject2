#include "SceneGame.h"
#include "SceneResult.h"
#include "SceneSystem/SceneManager.h"

#include "Camera/Camera.h"
#include "GameSystem/ScoreManager.h"

#include "GameObject/GameObjectManager.h"

#pragma region コンポーネントのインクルード
#include "Component/CompModel.h"
#include "Component/CompCollision.h"
#include "Component/CompPlayer.h"
#include "Component/CompStage.h"
#include "Component/CompGun.h"
#include "Component/CompSword.h"
#include "Component/CompStraightBullet.h"
#include "Component/CompSprite3D.h"
#include "Component/CompFontNumber.h"
#include "Component/CompFontTimeLimit.h"

#include "Component/CompScrollUI.h"
#include "Component/CompMagazineUI.h"
#include "Component/CompMagazineCoverUI.h"
#include "Component/CompBooleanSprite.h"
#include "Component/CompThrusterUI.h"
#include "Component/CompThrusterBaseUI.h"
#include "Component/CompSightUI.h"

#include "Component/CompStartUpScale.h"
#include "Component/CompStartUpScroll.h"

#include "Component/CompCameraPlayer.h"
#include "Component/CompCameraFree.h"

#include "Component/CompParticleEmitter.h"
#include "Component/CompParticleLandDustCtr.h"
#include "Component/CompParticleThrusterCtr.h"
#include "Component/CompParticleQuickBoostCtr.h"
#include "Component/CompParticleSlashCtr.h"
#include "Component/CompParticleOneShotCtr.h"

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

    // プレイヤーの右手武装
    {
      SharedObject obj = objectManager.Create(TASK_LAYER::SECOND);
      obj->SetName("PlayerRightWeapon");
      obj->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("RightWeapon")->worldTransform);
      obj->AddComponent<CompModel>("./Data/Model/RailGun/RailGun.model", &modelRenderer);

      std::shared_ptr<CompStraightBullet> bullet = std::make_shared<CompStraightBullet>(
        "./Data/Sprite/Effect/trailEffect2.png",  // 弾の見た目
        0.3f,                                     // 弾の見た目の大きさ
        320.0f,                                   // 攻撃力
        1.5f,                                     // 速度
        4.0f,                                     // 寿命
        0.3f);                                    // 無敵時間

      CompGun* weaponComp = obj->AddComponent<CompGun>(
        21,                           // マガジン容量
        3.2f,                         // リロード時間
        0.9f,                         // 連射速度
        COLLIDER_TAGS::ENEMY,         // 攻撃対象
        bullet).get();                // 弾丸

      player->GetComponent<CompPlayer>()->SetRightWeapon(weaponComp);

      // 射撃エフェクト
      {
        GameObject* effect = objectManager.Create(TASK_LAYER::THIRD).get();
        effect->SetName("Shot Particle");
        effect->transform.SetParentTransform(obj,
          &obj->GetComponent<CompModel>()->FindNode("WeaponNode")->worldTransform);
        CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(5000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
        effect->AddComponent<CompParticleOneShotCtr>(&weaponComp->GetExecuteFlag());

#pragma region パラメーター
        effect->transform.rotation.y = 1.0f;
        comp->SetIsLoop(false);
        comp->SetDuration(0.05f);
        comp->SetEmitRate(40);
        comp->SetEmitInterval(0.0f);
        comp->emitterData.gravityModifier = 0.01f;
        comp->emitterData.lifeTime = 0.1f;
        comp->emitterData.emissivePower = 0.5f;
        comp->emitterData.spawnRandScale = 0.4f;
        comp->emitterData.vecRandScale = 0.0003f;
        comp->emitterData.startSize = 0.07f;
        comp->emitterData.endSize = 0.01f;
        comp->emitterData.particleSpeed = 16.0f;
        comp->emitterData.speedRandScale = 2.8f;
        comp->emitterData.startSpeedRate = 1.0f;
        comp->emitterData.endSpeedRate = 0.7f;
        comp->emitterData.startColor = { 1.0f,0.02f,0.0f,1.0f };
        comp->emitterData.endColor = { 1.0f,0.0f,0.0f,1.0f };
#pragma endregion
      }
    }

    // プレイヤーの左手武装 ( 剣 )
    {
      SharedObject obj = objectManager.Create(TASK_LAYER::SECOND);
      obj->SetName("PlayerLeftWeapon");
      obj->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LeftWeapon")->worldTransform);
      obj->AddComponent<CompModel>("./Data/Model/Sword/Sword.model", &modelRenderer)->AddCompCollisions();
      CompSword* sword = obj->AddComponent<CompSword>(
        20.0f,                                      // トレイルエフェクトの幅
        3.0f,                                       // インターバル
        COLLIDER_TAGS::ENEMY,                       // 攻撃対象
        1650.0f,                                    // 攻撃力
        "./Data/Sprite/Effect/sword.png").get();    // トレイルエフェクトのテクスチャパス

      player->GetComponent<CompPlayer>()->SetLeftWeapon(sword);

      // エフェクト( 近接攻撃 )
      {
        SharedObject effect = objectManager.Create(TASK_LAYER::THIRD);
        effect->SetName("SwordEffect");
        effect->AddComponent<CompParticleSlashCtr>(player.get());
        CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(4000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
        effect->transform.SetParentTransform(obj);

#pragma region パラメーター
        effect->transform.position = { -78.5f,32.0f,-30.0f };
        effect->transform.rotation = { -0.144f,0.008f,0.63f,0.0f };

        comp->SetIsLoop(false);
        comp->SetEmitRate(22);
        comp->SetEmitInterval(0.0f);
        comp->emitterData.gravityModifier = 0.01f;
        comp->emitterData.lifeTime = 0.14f;
        comp->emitterData.emissivePower = 1.3f;
        comp->emitterData.spawnRandScale = 0.2f;
        comp->emitterData.vecRandScale = 0.0f;
        comp->emitterData.startSize = 0.02f;
        comp->emitterData.endSize = 0.005f;
        comp->emitterData.particleSpeed = 0.0f;
        comp->emitterData.speedRandScale = 6.5f;
        comp->emitterData.startSpeedRate = 4.8f;
        comp->emitterData.endSpeedRate = 0.2f;
        comp->emitterData.startColor = { 0.0f,0.4f,0.95f,1.0f };
        comp->emitterData.endColor = { 0.0f,0.97f,0.12f,0.0f };
#pragma endregion
      }
      {
        SharedObject effect = objectManager.Create(TASK_LAYER::THIRD);
        effect->SetName("SwordEffect");
        effect->AddComponent<CompParticleSlashCtr>(player.get());
        CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(8000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
        effect->transform.SetParentTransform(obj);

#pragma region パラメーター
        effect->transform.position = { -254.5f,-330.0f,1150.0f };
        effect->transform.rotation = { -0.05f,0.95f,0.0f,0.0f };

        comp->SetIsLoop(false);
        comp->SetEmitRate(12);
        comp->SetEmitInterval(0.0f);
        comp->emitterData.gravityModifier = 0.01f;
        comp->emitterData.lifeTime = 0.5f;
        comp->emitterData.emissivePower = 2.2f;
        comp->emitterData.spawnRandScale = 0.4f;
        comp->emitterData.vecRandScale = 0.0008f;
        comp->emitterData.startSize = 0.02f;
        comp->emitterData.endSize = 0.005f;
        comp->emitterData.particleSpeed = 0.0f;
        comp->emitterData.speedRandScale = 1.2f;
        comp->emitterData.startSpeedRate = 6.6f;
        comp->emitterData.endSpeedRate = -0.2f;
        comp->emitterData.startColor = { 0.0f,1.0f,0.45f,1.0f };
        comp->emitterData.endColor = { 0.0f,0.97f,0.12f,0.0f };
#pragma endregion
      }
    }

#pragma region 機体のパーティクルエフェクト
    // エフェクト( 後ろバーニア上 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("BackThrusterUP");
      effect->AddComponent<CompParticleThrusterCtr>(player.get())->SetAcceptRange(-0.28f);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(4000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Ob_U")->worldTransform);

#pragma region パラメーター
      effect->transform.position = { 0.0f,130.0f,0.0f };
      effect->transform.rotation = { 0.0f,0.49f,-0.872f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(21);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.15f;
      comp->emitterData.emissivePower = 3.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.003f;
      comp->emitterData.startSize = 0.02f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = -9.0f;
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
      effect->AddComponent<CompParticleThrusterCtr>(player.get())->SetAcceptRange(-0.28f);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(4000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,100.0f,0.0f };
      effect->transform.rotation = { 0.0f,0.3f,-0.955f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.0f);
      comp->SetEmitRate(21);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.10f;
      comp->emitterData.emissivePower = 4.0f;
      comp->emitterData.spawnRandScale = 0.07f;
      comp->emitterData.vecRandScale = 0.003f;
      comp->emitterData.startSize = 0.02f;
      comp->emitterData.endSize = 0.005f;
      comp->emitterData.particleSpeed = -10.0f;
      comp->emitterData.speedRandScale = 8.0f;
      comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
      comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
    }

    // エフェクト( クイックブースト時 ( 背中 ) )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("QuickBoostBack");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Ob_L")->worldTransform);
      effect->AddComponent<CompParticleQuickBoostCtr>(player.get());
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(5000, "./Data/Sprite/Effect/fireParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,100.0f,0.0f };
      effect->transform.rotation = { 0.0f,0.95f,-0.3f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.1f);
      comp->SetEmitRate(50);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.21f;
      comp->emitterData.emissivePower = 0.0f;
      comp->emitterData.spawnRandScale = 0.18f;
      comp->emitterData.vecRandScale = 0.0f;
      comp->emitterData.startSize = 0.12f;
      comp->emitterData.endSize = 0.02f;
      comp->emitterData.particleSpeed = -4.3f;
      comp->emitterData.speedRandScale = -6.0f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 1.0f;
      comp->emitterData.startColor = { 0.3f,0.2f,0.0f,0.3f };
      comp->emitterData.endColor = { 5.0f,0.0f,0.0f,0.1f };
#pragma endregion
    }

    // エフェクト( 左足 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("LeftLeg");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_L")->worldTransform);
      effect->AddComponent<CompParticleThrusterCtr>(player.get())->SetAcceptRange(0.6f);
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
    // エフェクト( クイックブースト時 ( 左足 ) )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("QuickBoostLeft");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_L")->worldTransform);
      effect->AddComponent<CompParticleQuickBoostCtr>(player.get())->SetAcceptRange(0.7f);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/fireParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,-130.0f,0.0f };
      effect->transform.rotation = { -0.4f, 0.39f,0.83f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.1f);
      comp->SetEmitRate(30);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.21f;
      comp->emitterData.emissivePower = 0.0f;
      comp->emitterData.spawnRandScale = 0.18f;
      comp->emitterData.vecRandScale = 0.001f;
      comp->emitterData.startSize = 0.12f;
      comp->emitterData.endSize = 0.02f;
      comp->emitterData.particleSpeed = -4.3f;
      comp->emitterData.speedRandScale = -6.0f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 1.0f;
      comp->emitterData.startColor = { 0.3f,0.2f,0.0f,0.3f };
      comp->emitterData.endColor = { 5.0f,0.0f,0.0f,0.1f };
#pragma endregion
    }

    // エフェクト( 右足 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("RightLeg");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_R")->worldTransform);
      effect->AddComponent<CompParticleThrusterCtr>(player.get())->SetAcceptRange(0.6f);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,-130.0f,0.0f };
      effect->transform.rotation = { 0.5f,-0.5f,0.0f,0.0f };

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
    // エフェクト( クイックブースト時 ( 右足 ) )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("QuickBoostRight");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("LegBoosterC_R")->worldTransform);
      effect->AddComponent<CompParticleQuickBoostCtr>(player.get())->SetAcceptRange(0.7f);
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/fireParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
      effect->transform.position = { 0.0f,-130.0f,0.0f };
      effect->transform.rotation = { 0.4f, 0.39f,0.83f,0.0f };

      comp->SetIsLoop(false);
      comp->SetDuration(0.1f);
      comp->SetEmitRate(30);
      comp->SetEmitInterval(0.0f);
      comp->emitterData.lifeTime = 0.21f;
      comp->emitterData.emissivePower = 0.0f;
      comp->emitterData.spawnRandScale = 0.18f;
      comp->emitterData.vecRandScale = 0.001f;
      comp->emitterData.startSize = 0.12f;
      comp->emitterData.endSize = 0.02f;
      comp->emitterData.particleSpeed = -4.3f;
      comp->emitterData.speedRandScale = -6.0f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 1.0f;
      comp->emitterData.startColor = { 0.3f,0.2f,0.0f,0.3f };
      comp->emitterData.endColor = { 5.0f,0.0f,0.0f,0.1f };
#pragma endregion
    }

    // 地面移動時のエフェクト ( 砂煙 )
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("LandDustParticle");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Rb_Toe_R")->worldTransform);
      effect->AddComponent<CompParticleLandDustCtr>(player.get());
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
      comp->emitterData.particleSpeed = 0.8f;
      comp->emitterData.speedRandScale = 1.5f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 0.2f;
      comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.1f };
      comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
    }
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::SECOND).get();
      effect->SetName("LandDustParticle");
      effect->transform.SetParentTransform(player, &player->GetComponent<CompModel>()->FindNode("Rb_Toe_L")->worldTransform);
      effect->AddComponent<CompParticleLandDustCtr>(player.get());
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
      comp->emitterData.particleSpeed = 0.8f;
      comp->emitterData.speedRandScale = 1.5f;
      comp->emitterData.startSpeedRate = 1.0f;
      comp->emitterData.endSpeedRate = 0.2f;
      comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.1f };
      comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
    }
#pragma endregion

    // カメラ
    {
      GameObject* camera = objectManager.Create(TASK_LAYER::THIRD).get();
      camera->SetName("Camera");
      camera->AddComponent<CompCameraPlayer>(weakPlayer);
      //camera->AddComponent<CompCameraFree>();
    }

    // ステージエフェクト
    {
      GameObject* effect = objectManager.Create(TASK_LAYER::FIRST).get();
      effect->SetName("StageParticle");
      // プレイヤーを中心にパーティクルを生成する
      effect->transform.SetParentTransform(player, &player->transform.GetWorldTransform());
      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(1500, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();

#pragma region パラメーター
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

  // 敵の生成
  enemySpawner.Initialize(this);

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

  // ゲーム開始演出
#if(1)
  {
    //------------------------------------------------
    //	 画面効果
    //------------------------------------------------
#if(1)
    {
      GameObject* ScreenEffect = objectManager.Create(TASK_LAYER::FIRST).get();
      ScreenEffect->SetName("ScreenEffect");
      CompSprite2D* sprite = ScreenEffect->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Effect/StartUpOverRay.png").get();
      ScreenEffect->transform.position = { 640.0f,380.0f,0.0f };
      ScreenEffect->transform.scale = { 5000.0f, 5000.0f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetDrawOrder(-1);

      ScreenEffect->AddComponent<CompBooleanSprite>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::LINE), 2.4f);
    }
#endif

  //------------------------------------------------
  //	 システムメッセージ
  //------------------------------------------------
#if(1)
    // 右側
    {
      GameObject* active = objectManager.Create(TASK_LAYER::FIRST).get();
      active->SetName("Message");
      CompSprite2D* sprite = active->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/tekitoEnglish.png").get();
      active->transform.position = { 1150.0f,370.0f,0.0f };
      active->transform.scale = { 0.55f, 0.8f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.4f });

      sprite->SetBsMode(BS_MODE::ADD);
      sprite->SetShaderID(SHADER_ID_SPRITE::BEND_SCREEN);

      active->AddComponent<CompStartUpScroll>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAIN_SYSTEM),
        DirectX::XMFLOAT2(0.0f, StartUpManager::MASSAGE_SCROLL_SPEED), StartUpManager::START_UP_MASSAGE);
    }
    // 左側
    {
      GameObject* active = objectManager.Create(TASK_LAYER::FIRST).get();
      active->SetName("Message");
      CompSprite2D* sprite = active->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/tekitoEnglish.png").get();
      active->transform.position = { 130.0f,370.0f,0.0f };
      active->transform.scale = { 0.55f, 0.8f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetTexSize({ sprite->GetTexSize().x, sprite->GetTexSize().y / 1.69f });
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.4f });

      sprite->SetBsMode(BS_MODE::ADD);
      sprite->SetShaderID(SHADER_ID_SPRITE::BEND_SCREEN);

      active->AddComponent<CompStartUpScroll>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAIN_SYSTEM),
        DirectX::XMFLOAT2(0.0f, StartUpManager::MASSAGE_SCROLL_SPEED), StartUpManager::START_UP_MASSAGE);
    }
#endif

    //------------------------------------------------
    //	 波形
    //------------------------------------------------
#if(1)
    {
      GameObject* wave = objectManager.Create(TASK_LAYER::FIRST).get();
      wave->SetName("Wave");
      CompSprite2D* sprite = wave->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/Wave.png").get();
      wave->transform.position = { 60.0f,450.0f,0.0f };
      wave->transform.scale = { 0.45f, 0.45f, 1.0f };
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.92f });

      sprite->SetBsMode(BS_MODE::ADD);

      wave->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::LINE),
        DirectX::XMFLOAT2(0.0f, 900.0f),
        StartUpManager::START_UP_SYSTEM_WAVE);

      wave->AddComponent<CompScrollUI>(
        &standbyTimer,
        true, 400.0f,
        DirectX::XMFLOAT2(1.0f, 0.0f));
    }
#endif

    //------------------------------------------------
    //	 フレーム
    //------------------------------------------------
#if(1)
    {
      GameObject* systemFrame = objectManager.Create(TASK_LAYER::FIRST).get();
      systemFrame->SetName("SystemFrame");
      CompSprite2D* sprite = systemFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/SystemFrame.png").get();
      systemFrame->transform.position = { 60.0f,450.0f,0.0f };
      systemFrame->transform.scale = { 0.45f, 0.45f, 1.0f };
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.92f });

      sprite->SetBsMode(BS_MODE::ADD);

      systemFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::LINE),
        DirectX::XMFLOAT2(0.0f, 900.0f),
        StartUpManager::START_UP_SYSTEM_WAVE);
    }
#endif

    //------------------------------------------------
    //	 横線
    //------------------------------------------------
#if(1)
    {
      GameObject* line = objectManager.Create(TASK_LAYER::FIRST).get();
      line->SetName("Line");
      CompSprite2D* sprite = line->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/line.png").get();
      line->transform.position = { 640.0f,360.0f,0.0f };
      line->transform.scale = { 1.0f, 1.0f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.92f });

      auto compStartUp = line->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::LINE),
        DirectX::XMFLOAT2(0.0f, 1.0f), StartUpManager::START_UP_LINE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::LINE, compStartUp);
    }
#endif

    //------------------------------------------------
    //	 メインシステム
    //------------------------------------------------
#if(1)
    {
      GameObject* system = objectManager.Create(TASK_LAYER::FIRST).get();
      system->SetName("MainSystem");
      CompSprite2D* sprite = system->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/mainSystem.png").get();
      system->transform.position = { 640.0f,340.0f,0.0f };
      system->transform.scale = { 1.0f, 1.0f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.92f });

      auto compStartUp = system->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAIN_SYSTEM),
        DirectX::XMFLOAT2(0.0f, 1.0f), StartUpManager::START_UP_SYSTEM_ACTIVE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::MAIN_SYSTEM, compStartUp);
    }
#endif

    //------------------------------------------------
    //	 アクティブ
    //------------------------------------------------
#if(1)
    {
      GameObject* active = objectManager.Create(TASK_LAYER::FIRST).get();
      active->SetName("Active");
      CompSprite2D* sprite = active->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/battleMode.png").get();
      active->transform.position = { 640.0f,380.0f,0.0f };
      active->transform.scale = { 1.0f, 1.0f, 1.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.92f });

      auto compStartUp = active->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MODE_ACTIVE),
        DirectX::XMFLOAT2(0.0f, 1.0f), StartUpManager::START_UP_SYSTEM_ACTIVE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::MODE_ACTIVE, compStartUp);
    }
#endif
  }
#endif

  // UI
#if(1)
  {
    //------------------------------------------------
    //	 体力ゲージ
    //------------------------------------------------
#if(1)
    {
      GameObject* healthGauge = objectManager.Create(TASK_LAYER::NONE).get();
      CompSprite2D* sprite = healthGauge->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthBase.png").get();
      healthGauge->transform.position = { 190.0f,660.0f,0 };
      healthGauge->transform.scale = { 0.6f,1.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.85f });

      healthGauge->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* healthGauge = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = healthGauge->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthBase2.png").get();
      healthGauge->transform.position = { 190.0f,660.0f,0 };
      healthGauge->transform.scale = { 0.6f,1.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.71f });

      healthGauge->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* damageGauge = objectManager.Create(TASK_LAYER::SECOND).get();
      damageGauge->AddComponent<CompScrollUI>(&compPlayer->uiParams.damage, false, 1.0f, DirectX::XMFLOAT2(1.0f, 0.0f));
      CompSprite2D* sprite = damageGauge->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthGauge.png").get();
      damageGauge->transform.position = { 190.0f,660.0f,0 };
      damageGauge->transform.scale = { 0.6f,1.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 5.0f,0.0f,0.0f,1.0f });

      damageGauge->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* healthGauge = objectManager.Create(TASK_LAYER::SECOND).get();
      healthGauge->AddComponent<CompScrollUI>(&compPlayer->uiParams.healthRate, false, 1.0f, DirectX::XMFLOAT2(1.0f, 0.0f));
      CompSprite2D* sprite = healthGauge->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthGauge.png").get();
      healthGauge->transform.position = { 190.0f,660.0f,0 };
      healthGauge->transform.scale = { 0.6f,1.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);

      healthGauge->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* apText = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = apText->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/AP.png").get();
      apText->transform.position = { 62.0f,625.0f,0 };
      apText->transform.scale = { 0.5f,0.5f,0.0f };
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      apText->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* value = objectManager.Create(TASK_LAYER::SECOND).get();
      CompFont* font = value->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 4).get();
      value->AddComponent<CompFontNumber>(&compPlayer->GetHealth(), 0, 4);
      value->transform.position = { 184.0f,595.0f,0 };
      value->transform.scale = { 0.74f,0.74f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      value->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 スラスターゲージ
    //------------------------------------------------
#if(1)
    {
      GameObject* thrusterFrame = objectManager.Create(TASK_LAYER::NONE).get();
      CompSprite2D* sprite = thrusterFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthBase.png").get();
      thrusterFrame->transform.position = { 640.0f,625.0f,0 };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.85f });

      thrusterFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* thrusterBase = objectManager.Create(TASK_LAYER::SECOND).get();
      thrusterBase->AddComponent<CompThrusterBaseUI>(&compPlayer->uiParams.thrusterQuantity);
      CompSprite2D* sprite = thrusterBase->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthBase2.png").get();
      thrusterBase->transform.position = { 640.0f,625.0f,0 };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.71f });

      thrusterBase->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* thrusterGauge = objectManager.Create(TASK_LAYER::SECOND).get();
      thrusterGauge->AddComponent<CompThrusterUI>(&compPlayer->uiParams.thrusterQuantity);
      CompSprite2D* sprite = thrusterGauge->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/HealthGauge.png").get();
      thrusterGauge->transform.position = { 640.0f,625.0f,0 };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);

      thrusterGauge->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* boostMode = objectManager.Create(TASK_LAYER::SECOND).get();
      boostMode->AddComponent<CompBooleanSprite>(&compPlayer->GetBoostMode(), 5.5f);
      CompSprite2D* sprite = boostMode->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/BoostOn.png").get();
      boostMode->transform.position = { 935.0f,625.0f,0 };
      boostMode->transform.scale = { 0.6f,0.6f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.8f,0.95f,0.886f,0.0f });

      boostMode->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 レティクル
    //------------------------------------------------
#if(1)
    {
      GameObject* overSight = objectManager.Create(TASK_LAYER::THIRD).get();
      overSight->SetName("OverSight");
      CompSprite2D* sprite = overSight->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/OverSight.png").get();
      overSight->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      overSight->transform.position = { 640.0f,360.0f,0.0f };
      overSight->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.8f,0.8f,0.8f,0.45f });

      auto compStartUp = overSight->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::SIGHT),
        DirectX::XMFLOAT2(0.9f, 0.9f), StartUpManager::START_UP_SIGHT);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::SIGHT, compStartUp);
    }
    {
      GameObject* magazineBase = objectManager.Create(TASK_LAYER::THIRD).get();
      magazineBase->SetName("MagazineBase");
      CompSprite2D* sprite = magazineBase->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MagazineSupport.png").get();
      magazineBase->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      magazineBase->transform.position = { 640.0f,360.0f,0.0f };
      magazineBase->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.8f,0.95f,0.886f,0.84f });

      auto compStartUp = magazineBase->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAGAZINE),
        DirectX::XMFLOAT2(0.2f, 0.2f), StartUpManager::START_UP_MAGAZINE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::MAGAZINE, compStartUp);
    }
    {
      GameObject* sight = objectManager.Create(TASK_LAYER::THIRD).get();
      sight->SetName("Sight");
      CompSprite2D* sprite = sight->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/Sight.png").get();
      sight->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, &compPlayer->uiParams.isTarget);
      sight->transform.position = { 640.0f,360.0f,0.0f };
      sight->transform.scale = { 0.1f,0.1f,0.1f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
    }
    {
      GameObject* rockOnSight = objectManager.Create(TASK_LAYER::THIRD).get();
      rockOnSight->SetName("RockOnSight");
      CompSprite2D* sprite = rockOnSight->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/RockOnSight.png").get();
      rockOnSight->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, &compPlayer->uiParams.isRockOn);
      rockOnSight->transform.position = { 640.0f,360.0f,0.0f };
      rockOnSight->transform.scale = { 0.2f,0.2f,0.2f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.0f,1.0f,0.71f,1.0f });
    }
#endif

    //------------------------------------------------
    //	 弾倉
    //------------------------------------------------
#if(1)
    {
      GameObject* coverRight = objectManager.Create(TASK_LAYER::THIRD).get();
      coverRight->SetName("MaskRight");
      CompSprite2D* sprite = coverRight->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MagazineCover.png").get();
      coverRight->AddComponent<CompMagazineCoverUI>(compPlayer, true);
      coverRight->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      coverRight->transform.position = { 640.0f,360.0f,0.0f };
      coverRight->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot({ 0.0f,sprite->GetTexSize().y });
      sprite->SetDsMode(DS_MODE::MASK);
    }
    {
      GameObject* magazineRight = objectManager.Create(TASK_LAYER::THIRD).get();
      CompSprite2D* sprite = magazineRight->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MagazineRight.png").get();
      magazineRight->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      magazineRight->AddComponent<CompMagazineUI>(compPlayer, true);
      magazineRight->transform.position = { 640.0f,360.0f,0.0f };
      magazineRight->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.84f });
      sprite->SetDsMode(DS_MODE::EXCLUSIVE);

      auto compStartUp = magazineRight->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAGAZINE),
        DirectX::XMFLOAT2(0.2f, 0.2f), StartUpManager::START_UP_MAGAZINE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::MAGAZINE, compStartUp);
    }
    {
      GameObject* coverLeft = objectManager.Create(TASK_LAYER::THIRD).get();
      coverLeft->SetName("MaskLeft");
      CompSprite2D* sprite = coverLeft->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MagazineCover.png").get();
      coverLeft->AddComponent<CompMagazineCoverUI>(compPlayer, false);
      coverLeft->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      coverLeft->transform.position = { 640.0f,360.0f,0.0f };
      coverLeft->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot(sprite->GetTexSize());
      sprite->SetDsMode(DS_MODE::MASK);
    }
    {
      GameObject* magazineLeft = objectManager.Create(TASK_LAYER::THIRD).get();
      CompSprite2D* sprite = magazineLeft->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MagazineLeft.png").get();
      magazineLeft->AddComponent<CompSightUI>(&compPlayer->uiParams.screenPos, nullptr);
      magazineLeft->AddComponent<CompMagazineUI>(compPlayer, false);
      magazineLeft->transform.position = { 640.0f,360.0f,0.0f };
      magazineLeft->transform.scale = { 0.6f,0.6f,0.6f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.84f });
      sprite->SetDsMode(DS_MODE::EXCLUSIVE);

      auto compStartUp = magazineLeft->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::MAGAZINE),
        DirectX::XMFLOAT2(0.2f, 0.2f), StartUpManager::START_UP_MAGAZINE);
      startUpManager.SetSprite(StartUpManager::START_UP_LIST::MAGAZINE, compStartUp);
    }

#endif

    //------------------------------------------------
    //	 高度メーター
    //------------------------------------------------
#if(1)
    {
      GameObject* elevationText = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = elevationText->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/Alt.png").get();
      elevationText->transform.position = { 1214.0f,230.0f,0.0f };
      elevationText->transform.scale = { 0.12f,0.12f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      elevationText->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* elevationFrame = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = elevationFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/ScrollGaugeFrame.png").get();
      elevationFrame->transform.position = { 1230.0f,360.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      elevationFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* valueFrame = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = valueFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MeterValueFrame.png").get();
      valueFrame->transform.position = { 1245.0f,360.0f,0.0f };
      valueFrame->transform.scale = { 0.56f,0.4f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      valueFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* value = objectManager.Create(TASK_LAYER::FIRST).get();
      CompFont* font = value->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 4).get();
      value->AddComponent<CompFontNumber>(&compPlayer->uiParams.elevation, 0);
      value->transform.position = { 1238.0f,340.0f,0.0f };
      value->transform.scale = { 0.22f,0.22f,0.0f };
      font->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      value->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* elevationScroll = objectManager.Create(TASK_LAYER::FIRST).get();
      elevationScroll->AddComponent<CompScrollUI>(&compPlayer->uiParams.elevation, true, 15.0f, DirectX::XMFLOAT2(0.0f, 1.0f));
      CompSprite2D* sprite = elevationScroll->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/VerticalPoints.png").get();
      elevationScroll->transform.position = { 1230.0f,346.5f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetTexSize({ 100.0f,348.0f });
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      elevationScroll->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 スピードメーター
    //------------------------------------------------
#if(1)
    {
      GameObject* speedText = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = speedText->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/Speed.png").get();
      speedText->transform.position = { 63.2f,230.0f,0.0f };
      speedText->transform.scale = { 0.12f,0.12f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      speedText->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* speedFrame = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = speedFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/ScrollGaugeFrameReverse.png").get();
      speedFrame->transform.position = { 47.0f,360.0f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      speedFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* valueFrame = objectManager.Create(TASK_LAYER::FIRST).get();
      CompSprite2D* sprite = valueFrame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/MeterValueFrame.png").get();
      valueFrame->transform.position = { 32.0f,360.0f,0.0f };
      valueFrame->transform.scale = { 0.56f,0.4f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });
      sprite->SetAngle(180);

      valueFrame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* value = objectManager.Create(TASK_LAYER::FIRST).get();
      CompFont* font = value->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 4).get();
      value->AddComponent<CompFontNumber>(&compPlayer->uiParams.moveSpeed, 0);
      value->transform.position = { 12.0f,340.0f,0.0f };
      value->transform.scale = { 0.22f,0.22f,0.0f };
      font->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      value->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* speedScroll = objectManager.Create(TASK_LAYER::FIRST).get();
      speedScroll->AddComponent<CompScrollUI>(&compPlayer->uiParams.moveSpeed, true, 1.8f, DirectX::XMFLOAT2(0.0f, 1.0f));
      CompSprite2D* sprite = speedScroll->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/VerticalPointsReverse.png").get();
      speedScroll->transform.position = { 47.0f,346.5f,0.0f };
      sprite->SetPivot(sprite->GetTexSize() / 2.0f);
      sprite->SetTexSize({ 100.0f,348.0f });
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.75f });

      speedScroll->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 タイムリミット
    //------------------------------------------------
#if(1)
    {
      GameObject* limit = objectManager.Create(TASK_LAYER::FIRST).get();
      limit->transform.position = { 0.0f,-15.0f,0.0f };
      CompSprite2D* sprite = limit->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/TimeLimit.png").get();
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.9f });

      limit->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* minute = objectManager.Create(TASK_LAYER::SECOND).get();
      minute->SetName("Minute");
      CompFont* font = minute->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      minute->AddComponent<CompFontNumber>(&uiMinute, 0, 2);
      minute->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      minute->transform.position = { 555.0f,57.0f,0 };
      minute->transform.scale = { 0.6f,0.6f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      minute->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, minute->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* second = objectManager.Create(TASK_LAYER::SECOND).get();
      second->SetName("Second");
      CompFont* font = second->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      second->AddComponent<CompFontNumber>(&uiSecond, 0, 2);
      second->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      second->transform.position = { 620.0f,57.0f,0 };
      second->transform.scale = { 0.6f,0.6f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      second->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, second->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* milliSecond = objectManager.Create(TASK_LAYER::SECOND).get();
      milliSecond->SetName("MilliSecond");
      CompFont* font = milliSecond->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      milliSecond->AddComponent<CompFontNumber>(&uiMilliSecond, 0, 2);
      milliSecond->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      milliSecond->transform.position = { 680.0f,72.0f,0 };
      milliSecond->transform.scale = { 0.3f,0.3f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      milliSecond->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, milliSecond->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 画面右下の残弾数
    //------------------------------------------------
#if(0)
    {
      GameObject* flame = objectManager.Create(TASK_LAYER::FIRST).get();
      flame->transform.position = { 0.0f,-15.0f,0.0f };
      CompSprite2D* sprite = flame->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/UI/TimeLimit.png").get();
      sprite->SetColor({ 1.0f,1.0f,1.0f,0.9f });

      flame->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* minute = objectManager.Create(TASK_LAYER::SECOND).get();
      minute->SetName("MagazineBullet");
      CompFont* font = minute->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      minute->AddComponent<CompFontNumber>(&uiMinute, 0, 2);
      minute->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      minute->transform.position = { 555.0f,57.0f,0 };
      minute->transform.scale = { 0.6f,0.6f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      minute->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, minute->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* second = objectManager.Create(TASK_LAYER::SECOND).get();
      second->SetName("Second");
      CompFont* font = second->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      second->AddComponent<CompFontNumber>(&uiSecond, 0, 2);
      second->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      second->transform.position = { 620.0f,57.0f,0 };
      second->transform.scale = { 0.6f,0.6f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      second->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, second->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* milliSecond = objectManager.Create(TASK_LAYER::SECOND).get();
      milliSecond->SetName("MilliSecond");
      CompFont* font = milliSecond->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      milliSecond->AddComponent<CompFontNumber>(&uiMilliSecond, 0, 2);
      milliSecond->AddComponent<CompFontTimeLimit>(&gameTimer, 16.0f);
      milliSecond->transform.position = { 680.0f,72.0f,0 };
      milliSecond->transform.scale = { 0.3f,0.3f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      milliSecond->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, milliSecond->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
#endif

    //------------------------------------------------
    //	 敵の撃破数
    //------------------------------------------------
#if(1)
    {
      GameObject* speedText = objectManager.Create(TASK_LAYER::FIRST).get();
      speedText->transform.position = { 12.0f,126.0f,0.0f };
      CompSprite2D* sprite = speedText->AddComponent<CompSprite2D>(&sprite2DRenderer, "./Data/Sprite/Text/AttackEnemy.png").get();
      sprite->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      speedText->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, 1.0f),
        StartUpManager::START_UP_OTHER);
    }
    {
      GameObject* count = objectManager.Create(TASK_LAYER::SECOND).get();
      count->SetName("KillCount");
      CompFont* font = count->AddComponent<CompFont>(&fontRenderer, "./Data/Font/russo_one.fnt", 2).get();
      count->AddComponent<CompFontNumber>(&ScoreManager::Instance().GetKillCount(), 0, 2);
      count->transform.position = { 96.8f,105.0f,0 };
      count->transform.scale = { 0.45f,0.45f,0.0f };
      font->SetColor({ 0.92f,1.0f,0.96f,0.84f });

      count->AddComponent<CompStartUpScale>(
        startUpManager.GetFlag(StartUpManager::START_UP_LIST::OTHER),
        DirectX::XMFLOAT2(0.0f, count->transform.scale.y),
        StartUpManager::START_UP_OTHER);
    }
#endif
  }
#endif
}

void SceneGame::Initialize()
{
  // スコアポイントを初期化
  ScoreManager::Instance().Initialize();
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

  // ゲーム開始演出
  startUpManager.Update(elapsedTime);

  fontRenderer.EnforceColorAlpha(startUpManager.StartUpComplete());

  // 敵の生成処理
  enemySpawner.Update(elapsedTime, this);

  // 制限時間の更新
  UpdateTimeLimit(elapsedTime);

  // シーン遷移 処理
  SceneChangeCheck();
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

void SceneGame::SceneChangeCheck()
{
  if (GetFadeOutFlag() == false) {
    WeakObject player = objectManager.GetCharacterManager().GetPlayer();

    // 制限時間、プレイヤーが死亡
    if (gameTimer <= 0.0f || player.expired()) {

      // もしプレイヤーが生存しているなら
      if (player.expired() == false) {
        CompPlayer* compPlayer = player.lock()->GetComponent<CompPlayer>().get();
        ScoreManager::Instance().SetPlayerHealthRate(compPlayer->uiParams.healthRate);
      }
      else {
        ScoreManager::Instance().SetPlayerHealthRate(0.0f);
      }

      SwitchFade();
    }
  }
  // フェードアウトが完了したら
  if (fadeSprite.lock()->GetIsDisplay()) {
    // リザルトシーンへ遷移
    SceneManager::Instance().ChangeScene(new SceneLoading(new SceneResult));
  }
}
