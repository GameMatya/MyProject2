#include "CompHelicopter.h"
#include "StateMachine/HelicopterStates.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/StageManager.h"
#include "Graphics/EffectManager.h"
#include "imgui.h"
#include <string>

void CompHelicopter::Start()
{
  // ���N���X��Start�֐�
  CompCharacter::Start();

  MaxHealth = 800.0f;
  health = MaxHealth;
  gravityAffect = 0.0f;
  moveSpeed = 7.5f;
  airControl = 0.8f;
  accelerate = 0.5f;

  // �X�e�[�g�}�V���ɃX�e�[�g��ǉ�
  {
    stateMachine.AddState(std::make_shared<HeliIdle>(this));
    stateMachine.AddState(std::make_shared<HeliWander>(this));
    stateMachine.AddState(std::make_shared<HeliPursuit>(this));
    stateMachine.AddState(std::make_shared<HeliAttack>(this));
    stateMachine.AddState(std::make_shared<HeliDeath>(this));

    // �����X�e�[�g��ݒ�
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
  // �v���C���[�����S������
  if (support.IsAlivePlayer() == false) {
    // ���݂̃X�e�[�g���A�U���E�ǐՂ�������
    if (stateMachine.GetCurrentState() == STATE_LIST::ATTACK ||
      stateMachine.GetCurrentState() == STATE_LIST::PURSUIT)
      // �p�j�X�e�[�g�֑J��
      stateMachine.ChangeState(STATE_LIST::WANDER);
  }

  // �X�e�[�g�}�V���̍X�V
  stateMachine.Update(elapsedTime);

  // �̗͂�����ꍇ�A���C�L���X�g��Y�����W�����߂�
  if (health > 0.0f) {
    CalcPositionY();
  }

  // ���G���ԍX�V
  UpdateInvincibleTimer(elapsedTime);

  // ���͏����X�V
  UpdateVelocity(elapsedTime);
}

void CompHelicopter::DrawImGui()
{
  CompCharacter::DrawImGui();

  ImGui::DragFloat("FlyHeight", &flyHeight, 0.1f);

  // �X�e�[�g�}�V��
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
