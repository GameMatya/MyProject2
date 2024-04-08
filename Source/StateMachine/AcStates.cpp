#include "AcStates.h"
#include "System/Input.h"
#include "Math/easing.h"

#pragma region �ҋ@

void AcIdle::Enter()
{
  // �����ł̈ړ��l���傫����΃u���[�L�X�e�[�g��
  if (fabs(owner->GetVelocity().x) + fabs(owner->GetVelocity().z) >= DASH_END_THRESHOLD)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::DECELERATION);
    return;
  }

  // �ҋ@�A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Idle"), true, 0.5f);
  // �u�[�X�g���[�h����
  owner->DeactiveBoostMode();
}

void AcIdle::Execute(const float& elapsedTime)
{
  // �ړ��X�e�[�g
  if (owner->InputMove())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    return;
  }

  // �W�����v�X�e�[�g
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // �N�C�b�N�u�[�X�g�X�e�[�g
  else if (owner->InputQuickBoost())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // �ߐڍU���X�e�[�g
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // �ˌ�����
  owner->InputRightHand();
}

#pragma endregion

#pragma region �ړ�

AcMove::AcMove(CompArmoredCore* owner) :State(owner)
{
  landSE = Audio::Instance().LoadAudioSource("./Data/Audio/SE/BoostMoveLand.wav");
}

void AcMove::Enter()
{
  // ���s��
  if (owner->GetBoostMode() == false)
  {
    moveSpeed = owner->GetMoveSpeed();
    animeID = owner->GetModel()->animator.FindAnimationId("Walk_Forward");
  }
  // �u�[�X�g�ړ���
  else
  {
    landSE->Play(true);
    moveSpeed = owner->GetBoostSpeed();
    animeID = owner->GetModel()->animator.FindAnimationId("Dash_Forward");
  }
}

void AcMove::Execute(const float& elapsedTime)
{
#pragma region �X�e�[�g�J�ڏ���
  // �u�[�X�g���[�h�ɐ؂�ւ������ړ��X�e�[�g��J�ڂ�����
  if (owner->InputBoostMode()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    return;
  }

  // �N�C�b�N�u�[�X�g�X�e�[�g
  else if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // �W�����v�X�e�[�g
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // �ߐڃX�e�[�g
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // �󒆃X�e�[�g
  else if (owner->GetIsGround() == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
    return;
  }

#pragma endregion

  // �ˌ�����
  owner->InputRightHand();

  // �ړ����͂��Ȃ���Αҋ@�X�e�[�g��
  if (owner->InputMove() == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }

  // �A�j���[�V�����Đ����x��ύX
  {
    // ���s�A�j���[�V�������x���̂�1.5�{�Œ���
    float animeSpeed = CalcAnimeSpeed(moveSpeed) * WALK_ANIM_PLAY_SPEED;
    owner->GetModel()->animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, animeSpeed);
  }

  // �ړ� + ���� + �A�j���[�V�����Đ�
  {
    // �ʏ펞
    if (IsLookTarget() == false) {
      owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
    }
    // ���b�N�I����
    else {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }

    // �ړ�����
    owner->Move(owner->GetInputVec(), moveSpeed);

    // �A�j���[�V���� ( �ړ������ɉ�����4�����A�j���[�V�����𐧌� )
    owner->AnimationFromVectors(owner->GetInputVec(), animeID, true);
  }
}

void AcMove::Exit()
{
  owner->GetModel()->animator.PlayAnimationBottomSub(-1);
  landSE->Stop();
}

bool AcMove::IsLookTarget()
{
  if (owner->IsRockOn())return true;

  // �U���Ώۂ����Ȃ�
  if (owner->GetAttackTarget().expired())return false;

  // �r���U���X�e�[�g�ɂȂ��Ă���
  if (owner->GetLeftArmSM().GetCurrentState() != CompArmoredCore::ARM_STATE::NONE)return true;
  if (owner->GetRightArmSM().GetCurrentState() != CompArmoredCore::ARM_STATE::NONE)return true;

  // �U���Ώۂ�����A�r���U���X�e�[�g����Ȃ�
  return false;
}

float AcMove::CalcAnimeSpeed(const float& baseSpeed)
{
  // �����ړ����x
  DirectX::XMFLOAT2 horizonVelocity = { owner->GetVelocity().x,owner->GetVelocity().z };
  float speedSq = Mathf::Dot(horizonVelocity, horizonVelocity);

  // �����1
  return min(speedSq / baseSpeed, 1);
}

#pragma endregion

#pragma region �N�C�b�N�u�[�X�g

AcQuickBoost::AcQuickBoost(CompArmoredCore* owner) :State(owner)
{
  se = Audio::Instance().LoadAudioSource("./Data/Audio/SE/QuickBoost.wav");
  se->setVolume(0.42f);
}

void AcQuickBoost::Enter()
{
  // �u�[�X�g���[�h���N��
  owner->ActiveBoostMode();

  // �ړ����͂�������ΑO�����ɐi��
  if (owner->InputMove()) {
    moveVector = Mathf::Normalize(owner->GetInputVec());
  }
  else {
    moveVector = owner->GetGameObject()->transform.forward;
  }

  // ���͕����ɍ������A�j���[�V�������Đ�����
  owner->AnimationFromVectors(Mathf::Normalize(moveVector), owner->GetModel()->animator.FindAnimationId("QuickBoost_Forward"), false, 0.1f);

  se->Play(false);
}

void AcQuickBoost::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // �A�j���[�V�������I��������ʂ̃X�e�[�g�ɑJ��
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    // �ړ��X�e�[�g
    if (owner->InputMove())
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
      return;
    }
    // �󒆃X�e�[�g
    else if (owner->GetIsGround() == false)
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
      return;
    }

    // �ҋ@�X�e�[�g
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }

  // �N�C�b�N�u�[�X�g���̋����𐧌� ( https://www.geogebra.org/graphing?lang=ja�ō쐬 �A�� = 1-(-x^(a)+1)^(b) )
  {
    const float a = 0.7f;
    const float b = 5.0f;
    float speedRate = 1 - powf(-powf(animator.GetAnimationRate(ModelAnimator::ANIM_AREA::BOTTOM), a) + 1, b);
    float moveSpeed = std::lerp(owner->GetBoostSpeed(), owner->GetBoostSpeed() * SPEED_RATE, speedRate);

    owner->SetVelocity(moveVector * moveSpeed);
  }
}

#pragma endregion

#pragma region ����

void AcDecele::Enter()
{
  // �u���[�L�A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Dash_Stopping"), true);

  currentFlame = 0;
}

void AcDecele::Execute(const float& elapsedTime)
{
  // �L�����Z������
  if (currentFlame > CANCEL_FLAME && owner->InputMove()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
  }

  // �N�C�b�N�u�[�X�g����
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
  }

  // �W�����v���͏���
  else if (owner->InputJump())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::JUMP);
    return;
  }

  // �U������
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // �e�ۓ���
  owner->InputRightHand();

  // ����
  float inertia = fabs(owner->GetVelocity().x) + fabs(owner->GetVelocity().z);

  // �����̊�����0�ɂȂ�����ҋ@�X�e�[�g��
  if (inertia == 0.0f)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }
  // �������ʏ펞�̈ړ��\���x�ȉ��ɂȂ��
  else if (inertia <= MOVABLE_SPEED) {
    // �ړ����͏���
    if (owner->InputMove())
    {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
      return;
    }
  }
  // �������傫����
  else
  {
    // �񃍃b�N�I�����ɐ��񂾂��s��
    if (owner->IsRockOn() == false)
      owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
  }

  currentFlame++;
}

#pragma endregion

#pragma region �W�����v

void AcJump::Enter()
{
  // Jump�A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Jump_Enter"), false);
}

void AcJump::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // �W�����v�J�n�A�j���[�V�������I��������󒆃X�e�[�g�ɑJ��
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::AERIAL);
    return;
  }

  // �W�����v�O
  if (hasJumped == false) {
    // �w��̃L�[�t���[���܂őҋ@
    if (animator.GetCurrentAnimationSeconds(ModelAnimator::ANIM_AREA::BOTTOM) < 0.16f)return;

    // �W�����v�x�N�g�����Z�o ( �ړ����͂�����ꍇ�A���͕����ɌX���� )
    DirectX::XMFLOAT3 jumpVector = owner->GetInputVec() * 0.5f + DirectX::XMFLOAT3(0, 1, 0);
    jumpVector = Mathf::Normalize(jumpVector);

    // �W�����v�͂�K�p
    owner->AddImpulse(jumpVector * owner->GetJumpSpeed());
    hasJumped = true;
  }
  // �W�����v��
  else
  {
    // �ړ�
    if (owner->InputMove()) {
      if (owner->GetBoostMode() == false) {
        owner->Move(owner->GetInputVec(), owner->GetMoveSpeed());
      }
      else {
        owner->Move(owner->GetInputVec(), owner->GetBoostSpeed());
      }
      if (owner->IsRockOn() == false) {
        owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
      }
    }
    // ���b�N�I����ԂȂ�G�̕���������
    if (owner->IsRockOn() == true) {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }
  }
}

void AcJump::Exit()
{
  hasJumped = false;
}

#pragma endregion

#pragma region ��

void AcAerial::Enter()
{
  // Jump�A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Jump_Loop"), true);
}

void AcAerial::Execute(const float& elapsedTime)
{
  // �N�C�b�N�u�[�X�g
  if (owner->InputQuickBoost())
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  // �ߐڍU������
  else if (owner->InputLeftHand())
  {
    if (owner->LeftHandExe() == true)
      return;
  }

  // �ˌ�����
  owner->InputRightHand();

  // �ړ�+����
  {
    // �ړ�
    if (owner->InputMove()) {
      if (owner->GetBoostMode() == false) {
        owner->Move(owner->GetInputVec(), owner->GetMoveSpeed());
      }
      else {
        owner->Move(owner->GetInputVec(), owner->GetBoostSpeed());
      }
      if (owner->IsRockOn() == false) {
        owner->Turn(elapsedTime, owner->GetInputVec(), owner->GetTurnSpeed());
      }
    }
    // ���b�N�I����ԂȂ�G�̕���������
    if (owner->IsRockOn() == true) {
      owner->Turn(elapsedTime, owner->GetAttackTargetVecXZ(), owner->GetTurnSpeed());
    }
  }

  // �㏸
  if (owner->InputRise(elapsedTime)) {
    owner->Rising(elapsedTime);
  }
}

#pragma endregion

#pragma region ���n

void AcLanding::Enter()
{
  // ���n�A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, owner->GetModel()->animator.FindAnimationId("Landing"), false);
}

void AcLanding::Execute(const float& elapsedTime)
{
  // �N�C�b�N�u�[�X�g�X�e�[�g
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
    return;
  }
}

#pragma endregion

#pragma region �ߐڍU��

void AcSlashApproach::Enter()
{
  const char* animeId = (isSecond) ? "Second_Approach" : "Slash_Approach";

  // ����ɋ߂Â��A�j���[�V�����Đ�  
  owner->GetModel()->animator.PlayAnimation(
    ModelAnimator::ANIM_AREA::BOTTOM,
    owner->GetModel()->animator.FindAnimationId(animeId),
    true);

  // �U���Ώێ擾
  attackTarget = owner->GetAttackTarget();

  sword = static_cast<CompSword*>(owner->GetLeftWeapon());

  owner->SetIsSlashEffect(true);

  // �X���X�^�[���� ( 1��ڂ̂� )
  if (isSecond == false) {
    owner->GetThrusterManager()->QuantitySubtract(owner->THRUSTER_COST_BLADE);
  }
}

void AcSlashApproach::Execute(const float& elapsedTime)
{
#pragma region �X�e�[�g�J�ڏ���
  // �N�C�b�N�u�[�X�g�ŃL�����Z���\
  if (owner->InputQuickBoost()) {
    owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
    return;
  }

  CompArmoredCore::MAIN_STATE executeState = (isSecond) ? CompArmoredCore::MAIN_STATE::SECOND_EXECUTE : CompArmoredCore::MAIN_STATE::SLASH_EXECUTE;

  // �U���Ώۂ�����ꍇ
  if (attackTarget.expired() == false) {
    DirectX::XMFLOAT3 targetVec = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
    float distanceSq = Mathf::Dot(targetVec, targetVec);

    // �G�̕����Ɍ���
    {
      owner->Turn(elapsedTime, GetAttackTargetVecXZ(), CompArmoredCore::ABRUPT_TURN_SPEED);
    }

    // �ڕW�ɋ߂Â�����U�����s
    if (distanceSq <= EXECUTE_DISTANCE * EXECUTE_DISTANCE) {
      owner->GetMainSM().ChangeState(executeState);
      return;
    }
  }
  // �ǐՎ��Ԃ�ChaseTime���z������ߐڍU���X�e�[�g��
  if (chaseTimer >= CHASE_TIME) {
    owner->GetMainSM().ChangeState(executeState);
    return;
  }

#pragma endregion

  // �ړ�����
  {
    DirectX::XMFLOAT3 moveVector = {};
    // �U���Ώۂ�����ꍇ
    if (attackTarget.expired() == false) {
      moveVector = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
      moveVector = Mathf::Normalize(moveVector);
    }
    else
    {
      moveVector = owner->GetGameObject()->transform.forward;
    }

    owner->SetVelocity(moveVector * owner->GetBoostSpeed() * SPEED_RATE);
  }

  sword->UpdateEffect();

  chaseTimer += elapsedTime;
}

void AcSlashApproach::Exit()
{
  // �d�͗L����
  if (owner->GetBoostMode())
    owner->SetGravityAffect(owner->BOOST_GRAVITY_AFFECT);
  else
    owner->SetGravityAffect(1.0f);

  owner->SetIsSlashEffect(false);

  chaseTimer = 0;
}

DirectX::XMFLOAT3 AcSlashApproach::GetAttackTargetVecXZ()
{
  DirectX::XMFLOAT3 targetVecNorm = attackTarget.lock()->transform.position - owner->GetGameObject()->transform.position;
  targetVecNorm.y = 0.0f;
  return Mathf::Normalize(targetVecNorm); // ���K��
}

void AcSlashExecute::Enter()
{
  const char* animeId = (isSecond) ? "Second_Execute" : "Slash_Execute";

  // �U������A�j���[�V�����Đ�  
  ModelAnimator& animator = owner->GetModel()->animator;
  animator.PlayAnimation(
    ModelAnimator::ANIM_AREA::BOTTOM,
    animator.FindAnimationId(animeId),
    false);
  animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, 1.5f);

  owner->SetVelocity(owner->GetVelocity() * DECELE_SCALE);

  sword = static_cast<CompSword*>(owner->GetLeftWeapon());
  sword->PlayAnimation();

  // �d�͖�����
  owner->SetGravityAffect(0);
}

void AcSlashExecute::Execute(const float& elapsedTime)
{
  owner->SetIsSlashEffect(false);

  if (owner->GetModel()->animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false)
  {
    if (owner->InputMove())
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::MOVE);
    else
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);

    return;
  }

  // �A�j���[�V�����L�����Z������
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::CANCEL_ACCEPT))
  {
    // �N�C�b�N�u�[�X�g�X�e�[�g
    if (owner->InputQuickBoost()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::QUICK_BOOST);
      return;
    }

    // 2��ڋߐړ���
    if (owner->InputLeftHand()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::SECOND_APPROACH);
      return;
    }

    // �ˌ�����
    if (owner->InputRightHand()) {
      owner->GetMainSM().ChangeState(CompArmoredCore::MAIN_STATE::IDLE);
      return;
    }
  }

  // �U������
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::ATTACK))
  {
    sword->Execute(nullptr);
  }

  // �G�t�F�N�g����
  if (owner->GetModel()->animator.CheckEvent(ModelAnimator::ANIM_AREA::BOTTOM, ANIMATION_EVENT::EFFECT_START))
  {
    sword->UpdateEffect();
    owner->SetIsSlashEffect(true);
  }
}

void AcSlashExecute::Exit()
{
  // �d�͗L����
  if (owner->GetBoostMode())
    owner->SetGravityAffect(owner->BOOST_GRAVITY_AFFECT);
  else
    owner->SetGravityAffect(1.0f);

  owner->SetIsSlashEffect(false);

  // �ߐڕ���̃N�[���^�C���ݒ�
  owner->GetLeftWeapon()->Reload();
}

#pragma endregion

void AcDeath::Enter()
{
  ModelAnimator& animator = owner->GetModel()->animator;

  // ���n�A�j���[�V�����Đ�  
  animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM,
    animator.FindAnimationId("Dead"), false);
  animator.SetAnimationSpeed(ModelAnimator::ANIM_AREA::BOTTOM, 1.2f);
}

void AcDeath::Execute(const float& elapsedTime)
{
  ModelAnimator& animator = owner->GetModel()->animator;
  if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM) == false) {
    owner->GetGameObject()->Destroy();
  }
}
