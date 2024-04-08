#pragma once

#include "CompCharacter.h"
#include "StateMachine/StateMachine.h"
#include "CompWeaponBase.h"
#include "ThrusterManager.h"
#include "Audio/Audio.h"

class CompArmoredCore : public CompCharacter
{
public:
  // ���C���̍s��
  enum class MAIN_STATE
  {
    IDLE,           // �ҋ@

    MOVE,           // ���s
    QUICK_BOOST,    // �}����
    DECELERATION,   // ����

    JUMP,           // �W�����v
    AERIAL,         // ��
    LANDING,        // ���n

    SLASH_APPROACH, // �ߐڍU������
    SLASH_EXECUTE,  // �ߐڍU�����s

    SECOND_APPROACH, // �ߐڍU������ ( 2��� )
    SECOND_EXECUTE,  // �ߐڍU�����s ( 2��� )

    DEATH,          // ���S
    DAMAGE,         // ��_���[�W

    MAX
  };

  // �r�̍s��
  enum class ARM_STATE
  {
    NONE = -1, // ����
    SHOT, // �ˌ�

    MAX
  };

  // �u�[�X�g�ړ����̏d�͉e���x
  const float BOOST_GRAVITY_AFFECT = 0.6f;
  // �U�����̐��񑬓x
  static constexpr float ABRUPT_TURN_SPEED = DirectX::XMConvertToRadians(900);

  // �ߐڍU���̃X���X�^�[�����
  const float THRUSTER_COST_BLADE = 23.0f;
  // �N�C�b�N�u�[�X�g�̃X���X�^�[�����
  const float THRUSTER_COST_QB = 23.0f;
  // �㏸���̃X���X�^�[�̏���x ( 1�b�Ԃ̏���� )
  const float THRUSTER_COST_RISE = 25.0f;

public:
  CompArmoredCore();

  virtual void Start()override;

  void DrawImGui()override;

  void Rising(const float& elapsedTime);

  // ������͂��������ۂ̏���
  bool LeftHandExe();

#pragma region ���͏��
  virtual void ActiveBoostMode();
  virtual void DeactiveBoostMode();
  virtual bool InputBoostMode() = 0;

  bool InputMove();
  virtual bool InputQuickBoost() = 0;
  virtual bool InputJump() = 0;
  virtual bool InputRise(const float& elapsedTime) = 0;
  virtual bool InputRightHand() = 0;
  virtual bool InputLeftHand() = 0;
#pragma endregion

#pragma region �Q�b�^�[�E�Z�b�^�[
  void SetRightWeapon(CompWeaponBase* weaponComp) { rightWeapon = weaponComp; }
  CompWeaponBase* GetRightWeapon() { return rightWeapon; }
  void SetLeftWeapon(CompWeaponBase* weaponComp) { leftWeapon = weaponComp; }
  CompWeaponBase* GetLeftWeapon() { return leftWeapon; }

  void SetIsSlashEffect(bool isEffect) { isSlashEffect = isEffect; }
  const bool& GetIsSlashEffect()const { return isSlashEffect; }

  StateMachine<CompArmoredCore, MAIN_STATE>& GetMainSM() { return mainStateMachine; };
  StateMachine<CompArmoredCore, ARM_STATE>& GetRightArmSM() { return armRightSM; };
  StateMachine<CompArmoredCore, ARM_STATE>& GetLeftArmSM() { return armLeftSM; };

  // �X�e�B�b�N���͒l����ړ��x�N�g�����擾
  const DirectX::XMFLOAT3& GetInputVec() const { return inputVector; }

  ThrusterManager* GetThrusterManager() { return &thrusterManager; }
  const bool& GetBoostMode()  const { return boostMode; }
  const float& GetBoostSpeed() const { return boostSpeed; }

  // ���b�N�I���t���O
  bool IsRockOn() const { return isRockOn; }

  // �U���Ώۂ̃A�h���X
  WeakObject GetAttackTarget() const { return attackTarget; }

  // �U���ΏۂɌ������x�N�g�����쐬
  DirectX::XMFLOAT3 GetAttackTargetVec() const;
  DirectX::XMFLOAT3 GetAttackTargetVecXZ() const;

#pragma endregion

protected:
  void OnLanding() override;

  virtual void OnDead() override;

protected:
  bool isRockOn = false;
  WeakObject attackTarget; // �U���Ώ�
  CompWeaponBase* rightWeapon = nullptr;
  CompWeaponBase* leftWeapon = nullptr;
  bool isSlashEffect = false;
  std::shared_ptr<Effect> deathEffect = nullptr;

  // �ړ����̓x�N�g��
  DirectX::XMFLOAT3 inputVector = {};

  // �u�[�X�g�ړ��t���O
  bool  boostMode = false;
  // �u�[�X�g�ړ����x
  float boostSpeed = 14.0f;
  // �X���X�^�[�Ǘ�
  ThrusterManager thrusterManager;

  // �㏸�X�s�[�h ( �S�@�̋��� )
  const float RISE_SPEED = 1.5f;
  const float MAX_RISE_SPEED = 8.0f;

  // �s������}�V��
  StateMachine<CompArmoredCore, MAIN_STATE>  mainStateMachine; // ���C���̍s����S��
  StateMachine<CompArmoredCore, ARM_STATE>   armRightSM;       // �E�r�̍s����S��
  StateMachine<CompArmoredCore, ARM_STATE>   armLeftSM;        // ���r�̍s����S��

  // �u�[�X�gSE
  std::unique_ptr<AudioSource> boosterSE;

};
