#include "CompArmoredCore.h"
#include "StateMachine/AcStates.h"
#include "StateMachine/AcArmStates.h"
#include "Graphics/EffectManager.h"

#include <memory>
#include <imgui.h>

CompArmoredCore::CompArmoredCore() :CompCharacter()
{
  deathEffect = EffectManager::Instance().LoadEffekseer("./Data/Effekseer/BombEnd.efk");
  assert(deathEffect != nullptr);

  boosterSE = Audio::Instance().LoadAudioSource("./Data/Audio/SE/Booster.wav");
  boosterSE->setVolume(0.4f);
}

void CompArmoredCore::Start()
{
  // ���N���X��Start�֐�
  CompCharacter::Start();

  // �X�e�[�g�}�V���ɃX�e�[�g��ǉ�
  {
    mainStateMachine.AddState(std::make_shared<AcIdle>(this));
    mainStateMachine.AddState(std::make_shared<AcMove>(this));
    mainStateMachine.AddState(std::make_shared<AcQuickBoost>(this));
    mainStateMachine.AddState(std::make_shared<AcDecele>(this));
    mainStateMachine.AddState(std::make_shared<AcJump>(this));
    mainStateMachine.AddState(std::make_shared<AcAerial>(this));
    mainStateMachine.AddState(std::make_shared<AcLanding>(this));
    mainStateMachine.AddState(std::make_shared<AcSlashApproach>(this, false));
    mainStateMachine.AddState(std::make_shared<AcSlashExecute>(this, false));
    mainStateMachine.AddState(std::make_shared<AcSlashApproach>(this, true));
    mainStateMachine.AddState(std::make_shared<AcSlashExecute>(this, true));
    mainStateMachine.AddState(std::make_shared<AcDeath>(this));

    // �����X�e�[�g��ݒ�
    mainStateMachine.ChangeState(MAIN_STATE::IDLE);
  }

  // �X�e�[�g�}�V���ɃX�e�[�g��ǉ�
  {
    if (rightWeapon != nullptr) {
      armRightSM.AddState(std::make_shared<AcArmShot>(this, ModelAnimator::ANIM_AREA::ARM_RIGHT));

      // �����X�e�[�g��ݒ�
      armRightSM.ChangeState(ARM_STATE::NONE);
    }

    if (leftWeapon != nullptr) {
      if (leftWeapon->IsSword() == false)
        armLeftSM.AddState(std::make_shared<AcArmShot>(this, ModelAnimator::ANIM_AREA::ARM_LEFT));

      // �����X�e�[�g��ݒ�
      armLeftSM.ChangeState(ARM_STATE::NONE);
    }
  }

  // �J�n�Ɉ�a�����������Ȃ��悤�ɏ����p����ݒ�
  {
    ModelAnimator& animator = model->animator;
    animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, animator.FindAnimationId("Idle"), true, 0.0f);
  }
}

void CompArmoredCore::DrawImGui()
{
  CompCharacter::DrawImGui();

  ImGui::Checkbox("BoostMode", &boostMode);

  ImGui::Separator();
  // �X�e�[�g�}�V��
  {
    const char* states[static_cast<int>(MAIN_STATE::MAX)] = {
      "IDLE",           // �ҋ@

      "MOVE",           // ���s
      "QUICK_BOOST",    // �}����
      "DECELERATION",   // ����

      "JUMP",           // �W�����v
      "AERIAL",         // ��
      "LANDING",        // ���n

      "SLASH_APPROACH", // �ߐڍU���ǔ�
      "SLASH_EXECUTE",  // �ߐڍU�����s

      //"DAMAGE",         // ��_���[�W
      "DEATH",          // ���S
    };

    MAIN_STATE state = mainStateMachine.GetCurrentState();
    ImGui::Text((std::string("MainState : ") + states[(int)state]).c_str());
  }
  ImGui::Separator();
  // �X�e�[�g�}�V��
  {
    const char* states[static_cast<int>(ARM_STATE::MAX) + 1] = {
      "NONE",           // �ҋ@
      "SHOT",           // �ˌ�
    };

    ARM_STATE state = armRightSM.GetCurrentState();
    ImGui::Text((std::string("ArmState : ") + states[(int)state + 1]).c_str());
  }
}

void CompArmoredCore::Rising(const float& elapsedTime)
{
  boostMode = true;

  // �o�߃t���[��
  float elapsedFrame = 60.0f * elapsedTime;
  velocity.y += RISE_SPEED * elapsedFrame;
  velocity.y = (std::min)(velocity.y, MAX_RISE_SPEED);
}

bool CompArmoredCore::LeftHandExe()
{
  if (leftWeapon->IsSword() == true) {
    mainStateMachine.ChangeState(CompArmoredCore::MAIN_STATE::SLASH_APPROACH);
    return true;
  }
  else {
    if (armLeftSM.GetCurrentState() != ARM_STATE::SHOT)
      armLeftSM.ChangeState(ARM_STATE::SHOT);
    return false;
  }
}

void CompArmoredCore::ActiveBoostMode()
{
  boostMode = true;
  // �d�͂̉e������߂�
  gravityAffect = BOOST_GRAVITY_AFFECT;

  boosterSE->Play(true);
}

void CompArmoredCore::DeactiveBoostMode()
{
  boostMode = false;
  // �d�͂̉e����ʏ�ɖ߂�
  gravityAffect = 1.0f;

  boosterSE->Stop();
}

bool CompArmoredCore::InputMove()
{
  return (inputVector.x != 0 || inputVector.z != 0);
}

DirectX::XMFLOAT3 CompArmoredCore::GetAttackTargetVec() const
{
  DirectX::XMFLOAT3 direction = attackTarget.lock()->transform.position - gameObject.lock()->transform.position;
  return direction;
}

DirectX::XMFLOAT3 CompArmoredCore::GetAttackTargetVecXZ() const
{
  DirectX::XMFLOAT3 targetVecNorm = GetAttackTargetVec();
  targetVecNorm.y = 0.0f;
  return Mathf::Normalize(targetVecNorm); // ���K��
}

void CompArmoredCore::OnLanding()
{
  // �ߐڍU���A�_���[�W�A���S�A�N�C�b�N�u�[�X�g ���͑J�ڂ��Ȃ��悤�ɂ���
  if (mainStateMachine.GetCurrentState() == MAIN_STATE::DAMAGE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::QUICK_BOOST ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_EXECUTE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SECOND_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SECOND_EXECUTE ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::DEATH) return;

  // �u�[�X�g�ړ����Ȃ�ړ��X�e�[�g��
  if (boostMode == true && InputMove()) {
    mainStateMachine.ChangeState(MAIN_STATE::MOVE);
  }
  // �������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��
  else if (velocity.y <= fallSpeed) {
    mainStateMachine.ChangeState(MAIN_STATE::LANDING);
  }
  else {
    mainStateMachine.ChangeState(MAIN_STATE::IDLE);
  }
}

void CompArmoredCore::OnDead()
{
  // ���S�X�e�[�g�֑J��
  mainStateMachine.ChangeState(MAIN_STATE::DEATH);

  if (deathEffect != nullptr)
    deathEffect->Play(
      GetGameObject()->transform.position + DirectX::XMFLOAT3(0, waistHeight, 0), // ���W + ���܂ł̍���
      0.3f // �X�P�[��
    );
}
