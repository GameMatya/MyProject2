#include "EnemySpawner.h"
#include "Math/OriginMath.h"
#include "Component/CompHelicopter.h"
#include "Component/CompEnemyAc.h"
#include "Component/CompGun.h"
#include "Component/CompPileBunker.h"
#include "Component/CompStraightBullet.h"

#include "Component/CompParticleEmitter.h"
#include "Component/CompParticleLandDustCtr.h"
#include "Component/CompParticleThrusterCtr.h"
#include "Component/CompParticleQuickBoostCtr.h"
#include "Component/CompParticleOneShotCtr.h"

void EnemySpawner::Initialize(Scene* scene)
{
  for (int i = 0; i < HELICOPTER_VOLUME; ++i) {
    SpawnHelicopter(scene);
  }
  SpawnAC(scene);
}

void EnemySpawner::Update(const float& elapsedTime, Scene* scene)
{
  auto enemies = scene->GetGameObjectManager().GetCharacterManager().GetEnemies();

  for (int i = static_cast<int>(enemies.size()); i < HELICOPTER_VOLUME; ++i) {
    SpawnHelicopter(scene);
  }
}

void EnemySpawner::SpawnHelicopter(Scene* scene)
{
  SharedObject enemy = scene->GetGameObjectManager().Create(TASK_LAYER::SECOND);
  enemy->SetName("Helicopter");
  enemy->transform.scale = HELICOPTER_SCALE;

  enemy->transform.position = TERRITORY_POS;
  float radius = SPAWN_DISTANCE / 2;
  enemy->transform.position.x += Mathf::RandomRange(-radius, radius);
  enemy->transform.position.z += Mathf::RandomRange(-radius, radius);

  enemy->SetPushPower(7.0f);

  auto heliComp = enemy->AddComponent<CompHelicopter>();
  heliComp->GetSupport()->SetTerritoryPosition(TERRITORY_POS);
  heliComp->GetSupport()->SetTerritoryRange(TERRITORY_RANGE);

  CompModel* model = enemy->AddComponent<CompModel>(
    "./Data/Model/Helicopter/helicopter.model",
    &scene->GetModelRenderer()).get();
  model->AddCompCollisions();
}

void EnemySpawner::SpawnAC(Scene* scene)
{
  GameObjectManager& objectManager = scene->GetGameObjectManager();

  SharedObject enemy = scene->GetGameObjectManager().Create(TASK_LAYER::SECOND);
  {
    enemy->SetName("EnemyAC");
    enemy->transform.scale = AC_SCALE;

    enemy->transform.position = TERRITORY_POS;
    float radius = SPAWN_DISTANCE / 2;
    enemy->transform.position.x += Mathf::RandomRange(-radius, radius);
    enemy->transform.position.z += Mathf::RandomRange(-radius, radius);

    enemy->SetPushPower(10.0f);

    auto enemyComp = enemy->AddComponent<CompEnemyAC>();
    enemyComp->GetSupport()->SetTerritoryPosition(TERRITORY_POS);
    enemyComp->GetSupport()->SetTerritoryRange(TERRITORY_RANGE);

    CompModel* model = enemy->AddComponent<CompModel>(
      "./Data/Model/Cavaliere/Cavaliere.model",
      &scene->GetModelRenderer()).get();
    model->AddCompCollisions();
  }

  // �ˌ�����
  {
    SharedObject obj = objectManager.Create(TASK_LAYER::THIRD);

    obj->transform.scale = AC_GUN_SCALE;

    obj->SetName("PlayerRightWeapon");
    obj->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("RightWeapon")->worldTransform);
    obj->AddComponent<CompModel>("./Data/Model/EnergyMachineGun/EnergyMachineGun.model", &scene->GetModelRenderer());

    std::shared_ptr<CompStraightBullet> bullet = std::make_shared<CompStraightBullet>(
      "./Data/Sprite/Effect/trailEffect.png",   // �e�̌�����
      0.3f,                                     // �e�̌����ڂ̑傫��
      220.f,                                    // �U����
      1.6f,                                     // ���x
      4.0f,                                     // ����
      0.3f);                                    // ���G����

    CompGun* weaponComp = obj->AddComponent<CompGun>(
      32,                           // �}�K�W���e��
      3.2f,                         // �����[�h����
      1.6f,                         // �A�ˑ��x
      COLLIDER_TAGS::PLAYER,        // �U���Ώ�
      bullet).get();                // �e��

    enemy->GetComponent<CompEnemyAC>()->SetRightWeapon(weaponComp);

    //    // �ˌ��G�t�F�N�g
    //    {
    //      GameObject* effect = objectManager.Create(TASK_LAYER::FOUR).get();
    //      effect->SetName("Shot Particle");
    //      effect->transform.SetParentTransform(obj,
    //        &obj->GetComponent<CompModel>()->FindNode("WeaponNode")->worldTransform);
    //      CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(5000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
    //      effect->AddComponent<CompParticleOneShotCtr>(&weaponComp->GetExecuteFlag());
    //
    //#pragma region �p�����[�^�[
    //      effect->transform.rotation.y = 1.0f;
    //      comp->SetIsLoop(false);
    //      comp->SetDuration(0.05f);
    //      comp->SetEmitRate(40);
    //      comp->SetEmitInterval(0.0f);
    //      comp->emitterData.gravityModifier = 0.01f;
    //      comp->emitterData.lifeTime = 0.1f;
    //      comp->emitterData.emissivePower = 0.5f;
    //      comp->emitterData.spawnRandScale = 0.4f;
    //      comp->emitterData.vecRandScale = 0.0003f;
    //      comp->emitterData.startSize = 0.07f;
    //      comp->emitterData.endSize = 0.01f;
    //      comp->emitterData.particleSpeed = 16.0f;
    //      comp->emitterData.speedRandScale = 2.8f;
    //      comp->emitterData.startSpeedRate = 1.0f;
    //      comp->emitterData.endSpeedRate = 0.7f;
    //      comp->emitterData.startColor = { 1.0f,0.02f,0.0f,1.0f };
    //      comp->emitterData.endColor = { 1.0f,0.0f,0.0f,1.0f };
    //#pragma endregion
    //    }
  }

  // �v���C���[�̍��蕐�� ( �� )
  {
    SharedObject obj = objectManager.Create(TASK_LAYER::THIRD);

    obj->transform.position = { 0.0f,10.0f,0.0f };

    obj->SetName("PileBunker");
    obj->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("LeftWeapon")->worldTransform);
    std::weak_ptr<CompModel> model = obj->AddComponent<CompModel>("./Data/Model/PileBunker/PileBunker.model", &scene->GetModelRenderer());
    model.lock()->AddCompCollisions();

    // �����p�����A�j���[�V�����̎p���ƈႤ�̂ŁA���O�Ɉ�񂾂��A�j���[�V�������Đ����Ă���
    model.lock()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, 0, false);

    CompPileBunker* pile = obj->AddComponent<CompPileBunker>(
      4.0f,                                       // �C���^�[�o��
      COLLIDER_TAGS::PLAYER,                      // �U���Ώ�
      1800.0f                                     // �U����
    ).get();

    enemy->GetComponent<CompEnemyAC>()->SetLeftWeapon(pile); scene->GetModelRenderer();
  }

#pragma region �@�̂̃p�[�e�B�N���G�t�F�N�g
  // �G�t�F�N�g( ���o�[�j�A )
  {
    GameObject* effect = objectManager.Create(TASK_LAYER::THIRD).get();
    effect->SetName("BackThrusterUP");
    effect->AddComponent<CompParticleThrusterCtr>(enemy.get())->SetAcceptRange(0.4f);
    CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(2000, "./Data/Sprite/Effect/particle.png", "Shader/EmitCornCS.cso").get();
    effect->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("spine_03")->worldTransform);

#pragma region �p�����[�^�[
    effect->transform.position = { -37.0f,0.0f,0.0f };
    effect->transform.rotation = { -0.7f,0.0f,-0.3f,0.0f };

    comp->SetIsLoop(false);
    comp->SetDuration(0.0f);
    comp->SetEmitRate(10);
    comp->SetEmitInterval(0.0f);
    comp->emitterData.lifeTime = 0.15f;
    comp->emitterData.emissivePower = 3.0f;
    comp->emitterData.spawnRandScale = 0.07f;
    comp->emitterData.vecRandScale = 0.003f;
    comp->emitterData.startSize = 0.04f;
    comp->emitterData.endSize = 0.005f;
    comp->emitterData.particleSpeed = -9.0f;
    comp->emitterData.speedRandScale = 8.0f;
    comp->emitterData.startColor = { 1.0f,0.001f,0.0f,1.0f };
    comp->emitterData.endColor = { 1.0f,0.2f,0.0f,0.0f };
#pragma endregion
  }

  // �G�t�F�N�g( �N�C�b�N�u�[�X�g�� ( �w�� ) )
  {
    GameObject* effect = objectManager.Create(TASK_LAYER::THIRD).get();
    effect->SetName("QuickBoostBack");
    effect->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("spine_03")->worldTransform);
    effect->AddComponent<CompParticleQuickBoostCtr>(enemy.get());
    CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(3000, "./Data/Sprite/Effect/fireParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region �p�����[�^�[
    effect->transform.position = { 0.0f,0.0f,0.0f };
    effect->transform.rotation = { -0.7f,0.0f,-0.3f,0.0f };

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

  // �n�ʈړ����̃G�t�F�N�g ( ���� )
  {
    GameObject* effect = objectManager.Create(TASK_LAYER::THIRD).get();
    effect->SetName("LandDustParticle");
    effect->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("ball_r")->worldTransform);
    effect->AddComponent<CompParticleLandDustCtr>(enemy.get());
    CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(2000, "./Data/Sprite/Effect/landParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region �p�����[�^�[
    effect->transform.position = { 0.0f,70.0f,0.0f };
    effect->transform.rotation = { 1.0f,0.0f,0.0f,0.0f };

    comp->SetBsMode(BS_MODE::ALPHA);
    comp->SetIsLoop(true);
    comp->SetDuration(0.0f);
    comp->SetEmitRate(6);
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
    comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.07f };
    comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
  }
  {
    GameObject* effect = objectManager.Create(TASK_LAYER::THIRD).get();
    effect->SetName("LandDustParticle");
    effect->transform.SetParentTransform(enemy, &enemy->GetComponent<CompModel>()->FindNode("ball_l")->worldTransform);
    effect->AddComponent<CompParticleLandDustCtr>(enemy.get());
    CompParticleEmitter* comp = effect->AddComponent<CompParticleEmitter>(2000, "./Data/Sprite/Effect/landParticle.png", "Shader/EmitCornCS.cso").get();

#pragma region �p�����[�^�[
    effect->transform.position = { 0.0f,70.0f,0.0f };
    effect->transform.rotation = { 1.0f,0.0f,0.0f,0.0f };

    comp->SetBsMode(BS_MODE::ALPHA);
    comp->SetIsLoop(true);
    comp->SetDuration(0.0f);
    comp->SetEmitRate(6);
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
    comp->emitterData.startColor = { 0.7f,0.2f,0.04f,0.07f };
    comp->emitterData.endColor = { 0.72f,0.25f,0.08f,0.0f };
#pragma endregion
  }
#pragma endregion

}
