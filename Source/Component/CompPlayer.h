#pragma once

#include "CompCharacter.h"
#include "Graphics/TrailEffect.h"
#include "StateMachine/StateMachine.h"

class CompPlayer :public CompCharacter
{
public:
  // ���C���̍s��
  enum class ACTION_STATE
  {
    IDLE,             // �ҋ@

    MOVE,             // �ړ�
    DODGE,            // ���

    MOVE_WEAPON,      // ������Ԃł̈ړ�
    ATTACK,           // �U��
    ATTACK_LOOP,      // �U��
    ATTACK_TRANS,     // �ό`�U��

    DEATH,            // ���S
    DAMAGE,           // ��_���[�W

    MAX
  };

  // �v���C���[�̏�
  enum class CONDITION
  {
    NORMAL,
    USE_ITEM,
    WEAPON_AXE,
    WEAPON_SWORD,
  };

  enum class ATTACK_ACTION
  {
    EMPTY,
    SHEATH, // �[����Ԃ̑ҋ@

    // ���̍U���A�N�V����
    AXE_IDLE,
    AXE_DASH,

    COMBO_AXE_Y1,
    COMBO_AXE_Y2,
    COMBO_AXE_Y3,

    AXE_LOOP,
    AXE_LOOP_END,

    // ���̍U���A�N�V����
    SWORD_IDLE,
    COMBO_SWORD_Y1,
    COMBO_SWORD_Y2,
    COMBO_SWORD_Y3,

    COMBO_SWORD_B1,
    COMBO_SWORD_B2,
    COMBO_SWORD_B3,
  };

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override;

  // ���N���X��Move�֐������b�v
  void Move(DirectX::XMFLOAT3 direction);

  void ResetDashFlg();

  StateMachine<CompPlayer, ACTION_STATE>& GetActionSM() { return actionStateMachine; }

  ATTACK_ACTION GetAttackAction() { return attackAction; }
  std::string GetAttackAnimeName() { return attackAnimeNames[attackAction]; }
  std::array<ATTACK_ACTION, 2> GetAttackSequence() { return attackSequence[attackAction]; }
  void SetAttackAction(ATTACK_ACTION action) { attackAction = action; }

  CONDITION GetCondition() { return condition; }
  void SetCondition(CONDITION condition) { this->condition = condition; }

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  DirectX::XMFLOAT3 GetInputVec() { return inputVector; }

  bool IsRockOn() { return isRockOn; };

  bool InputDodge();
  bool InputDash();
  bool InputWeaponSheathAndDraw();
  bool InputAttack();
  bool InputButtonY();
  bool InputButtonB();
  bool InputWeaponTransform();

private:
  inline void InitAttackContainer();

  // ���̓x�N�g���̍X�V
  void UpdateInputVector();

private:
  // ��s���͎󂯕t���t���[��
  const int EARLY_INPUT_RECEIPT_FRAME = 15;

  // �ړ����x
  const float MOVE_SPEED_NO_WEAPON = 16.0f;
  const float MOVE_SPEED_DASH = 32.0f;
  const float MOVE_SPEED_USE_ITEM = 13.0f;
  const float MOVE_SPEED_AXE = 12.5f;
  const float MOVE_SPEED_SWORD = 11.5f;

  bool isRockOn = false;
  bool dashFlg = false;
  bool dashThumbFlg = false;
  bool isDrawingSword = false;

  // �U���h���̃R���e�i �z��: 0 = Y�h�� , 1 = B�h��
  ATTACK_ACTION attackAction = ATTACK_ACTION::EMPTY;
  std::map<ATTACK_ACTION, std::array<ATTACK_ACTION, 2>> attackSequence;
  std::map<ATTACK_ACTION, std::string> attackAnimeNames;

  // �v���C���[�̏�
  CONDITION condition = CONDITION::NORMAL;

  std::weak_ptr<CompCharacter> attackTarget;

  // �X�e�B�b�N�̓��͕���
  DirectX::XMFLOAT3 inputVector = {};

  // �s������}�V��
  StateMachine<CompPlayer, ACTION_STATE>  actionStateMachine; // ���C���̍s����S��
};
