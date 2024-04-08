#include "CompHelicopter.h"
#include "StateMachine/HelicopterStates.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/StageManager.h"
#include "Graphics/EffectManager.h"
#include "imgui.h"
#include <string>

void CompHelicopter::Start()
{
  // 基底クラスのStart関数
  CompCharacter::Start();

  MaxHealth = 800.0f;
  health = MaxHealth;
  gravityAffect = 0.0f;
  moveSpeed = 7.5f;
  airControl = 0.8f;
  accelerate = 0.5f;

  // ステートマシンにステートを追加
  {
    stateMachine.AddState(std::make_shared<HeliIdle>(this));
    stateMachine.AddState(std::make_shared<HeliWander>(this));
    stateMachine.AddState(std::make_shared<HeliPursuit>(this));
    stateMachine.AddState(std::make_shared<HeliAttack>(this));
    stateMachine.AddState(std::make_shared<HeliDeath>(this));

    // 初期ステートを設定
    stateMachine.ChangeState(STATE_LIST::IDLE);
  }

  support.Initialize(gameObject);
  support.SetTerritoryRange(6.0f);
  support.SetSearchRange(25.0f);

  ModelAnimator& animator = model->animator;
  animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animator.FindAnimationId("Fly"), true);

  forwardAnimeId = animator.FindAnimationId("FlyForward");

  deathEffect = EffectManager::Instance().LoadEffekseer("./Data/Effekseer/HeliBomb.efk");
  assert(deathEffect != nullptr);
}

void CompHelicopter::Update(const float& elapsedTime)
{
  // プレイヤーが死亡したら
  if (support.IsAlivePlayer() == false) {
    // 現在のステートが、攻撃・追跡だったら
    if (stateMachine.GetCurrentState() == STATE_LIST::ATTACK ||
      stateMachine.GetCurrentState() == STATE_LIST::PURSUIT)
      // 徘徊ステートへ遷移
      stateMachine.ChangeState(STATE_LIST::WANDER);
  }

  // ステートマシンの更新
  stateMachine.Update(elapsedTime);

  // 体力がある場合、レイキャストでY軸座標を決める
  if (health > 0.0f) {
    CalcPositionY();
  }

  // 無敵時間更新
  UpdateInvincibleTimer(elapsedTime);

  // 速力処理更新
  UpdateVelocity(elapsedTime);
}

void CompHelicopter::DrawImGui()
{
  CompCharacter::DrawImGui();

  ImGui::DragFloat("FlyHeight", &flyHeight, 0.1f);

  // ステートマシン
  {
    const char* states[static_cast<int>(STATE_LIST::MAX)] = {
      "IDLE",
      "WANDER",
      "PURSUIt",
      "ATTACK",
      "DEAD"
    };

    STATE_LIST state = stateMachine.GetCurrentState();
    ImGui::Text((std::string("State : ") + states[(int)state]).c_str());
  }
}

void CompHelicopter::CalcPositionY()
{
  StageManager& manager = gameObject.lock()->GetObjectManager()->GetStageManager();
  ObjectTransform& transform = gameObject.lock()->transform;

  HitResult result = {};
  if (manager.Collision(transform.position + DirectX::XMFLOAT3(0, 0, 0),
    transform.position - DirectX::XMFLOAT3(0, 100, 0), &result)) {
    transform.position.y = std::lerp(transform.position.y, result.position.y + flyHeight, 0.8f);
  }
}

void CompHelicopter::OnLanding()
{
  gameObject.lock()->Destroy();

  deathEffect->Play(gameObject.lock()->transform.position, 0.3f);
}

void CompHelicopter::OnDead()
{
  ScoreManager::Instance().AddPoint(SCORE_HELICOPTER);

  stateMachine.ChangeState(STATE_LIST::DEAD);
}
