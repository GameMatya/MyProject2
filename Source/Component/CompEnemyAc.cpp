#include "CompEnemyAc.h"
#include "Scenes/SceneGame.h"
#include "GameObject/GameObjectManager.h"
#include "Math/OriginMath.h"
#include "StateMachine/EnemyAcStates.h"
#include "imgui.h"

void CompEnemyAC::Start()
{
  // ���N���X��Start�֐�
  CompArmoredCore::Start();

  // �p�����[�^�[�ݒ�
  moveSpeed = 5.0f;
  boostSpeed = 14.5f;
  jumpSpeed = 14.0f;
  fallSpeed = -15.0f;
  turnSpeed = DirectX::XMConvertToRadians(400);
  MaxHealth = 5900;
  health = MaxHealth;

  // �X�e�[�g�}�V���ɃX�e�[�g��ǉ�
  {
    actionSM.AddState(std::make_shared<EnemyAcPursuit>(this));
    actionSM.AddState(std::make_shared<EnemyAcRunAway>(this));
    actionSM.AddState(std::make_shared<EnemyAcInBattleRange>(this));
    actionSM.AddState(std::make_shared<EnemyAcSlashApproach>(this));

    // �����X�e�[�g��ݒ�
    actionSM.ChangeState(ACTION_STATE::PURSUIT);
  }

  support.Initialize(gameObject);

  isRockOn = true;
  attackTarget = support.GetPlayer();
}

void CompEnemyAC::Update(const float& elapsedTime)
{
  // �Q�[���̑ҋ@���Ԓ��͏������Ȃ�
  if (SceneGame::Instance()->StandbyComplete()) 
  {
    // �s���r�b�g���N���A
    actionFlagBit = 0;

    // �s���̃N�[���^�C���X�V
    actionCoolTimer -= elapsedTime;
    shootingTimer -= elapsedTime;

    // �X�e�[�g�}�V���̍X�V
    if (support.IsAlivePlayer() == true)
    {
      actionSM.Update(elapsedTime);
      mainStateMachine.Update(elapsedTime);
      armRightSM.Update(elapsedTime);
      armLeftSM.Update(elapsedTime);
    }
  }

  // ���G���ԍX�V
  UpdateInvincibleTimer(elapsedTime);

  // ���͏����X�V
  UpdateVelocity(elapsedTime);

  // �X���X�^�[�����X�V
  thrusterManager.Update(elapsedTime, isGround);
}

void CompEnemyAC::DrawImGui()
{
  CompCharacter::DrawImGui();

  int stateID = (int)actionSM.GetCurrentState();
  ImGui::InputInt("Action State", &stateID);
}

bool CompEnemyAC::InputBoostMode()
{
  boostMode = true;
  return false;
}

bool CompEnemyAC::InputQuickBoost()
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (~actionFlagBit & (1 << (int)ACTION_BIT::QUICK_BOOST))return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_QB);
  return true;
}

bool CompEnemyAC::InputJump()
{
  return actionFlagBit & (1 << (int)ACTION_BIT::JUMP);
}

bool CompEnemyAC::InputRise(const float& elapsedTime)
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (~actionFlagBit & (1 << (int)ACTION_BIT::JUMP))return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_RISE * elapsedTime);
  return true;
}

// �U�����͏���
bool CompEnemyAC::InputRightHand()
{
  if (shootingTimer > 0) {
    if (armRightSM.GetCurrentState() != ARM_STATE::SHOT)
      armRightSM.ChangeState(ARM_STATE::SHOT);

    return true;
  }
  else
    return false;
}

bool CompEnemyAC::InputLeftHand()
{
  if (thrusterManager.IsOverHeat() == true)return false;

  // ���蕐�킪�e�������ꍇ
  if (leftWeapon != nullptr && leftWeapon->IsSword() == false) {
    if (~actionFlagBit & (1 << (int)ACTION_BIT::LEFT_ATTACK))
      return false;

    return true;
  }

  // ���蕐�킪�ߐڕ��킾�����ꍇ
  else {
    if (~actionFlagBit & (1 << (int)ACTION_BIT::LEFT_ATTACK))return false;
    if (thrusterManager.IsOverHeat() == true)return false;
    if (leftWeapon->CanExecute() == false)return false;

    thrusterManager.QuantitySubtract(THRUSTER_COST_BLADE);
    ActiveBoostMode();
  }

  return true;
}

void CompEnemyAC::SetInputMoveVec(const DirectX::XMFLOAT3& inputMove)
{
  // �O�t���[���̓��͕����Ǝ��̓��͕����̍����傫���قǁA
  // ���`��Ԃ̕�ԗ����������Ȃ� ( �G�̓��������R�ɂ���� )
  float applyRate = Mathf::Dot(inputVector, inputMove) + 1.0f;
  applyRate /= 2.0f;
  inputVector = Mathf::Lerp(inputVector, inputMove, applyRate);
}

void CompEnemyAC::RandomAction(const float& gunRate, const float& bladeRate, const float& jumpRate, const float& dushRate)
{
  if (actionCoolTimer > 0)return;

  float random = Mathf::RandomRange(0, 100);

  // �N�[���^�C����ݒ�
  actionCoolTimer = Mathf::RandomRange(ACTION_COOL_TIME_MIN, ACTION_COOL_TIME_MAX);

  // �ˌ�
  if (random < gunRate) {
    InputShot();
    return;
  }
  // �ߐڍU��
  if (random < bladeRate + gunRate) {
    if (mainStateMachine.GetCurrentState() != MAIN_STATE::SLASH_EXECUTE) {
      actionSM.ChangeState(ACTION_STATE::SLASH_APPROACH);
      return;
    }
  }

  // �W�����v
  if (random < jumpRate + bladeRate + gunRate) {
    ActionBitStand(ACTION_BIT::JUMP);
    return;
  }
  // �N�C�b�N�u�[�X�g
  if (random < dushRate + jumpRate + bladeRate + gunRate) {
    ActionBitStand(ACTION_BIT::QUICK_BOOST);
    return;
  }
}

void CompEnemyAC::InputShot()
{
  // ���łɎˌ����������ꍇ�́A�������Ȃ�
  if (shootingTimer > 0)return;

  // �ˌ����Ԃ�ݒ�
  shootingTimer = Mathf::RandomRange(SHOOTING_TIME_MIN, SHOOTING_TIME_MAX);
}

void CompEnemyAC::OnDead()
{
  CompArmoredCore::OnDead();

  ScoreManager::Instance().AddPoint(SCORE_ENEMY_AC);
}
