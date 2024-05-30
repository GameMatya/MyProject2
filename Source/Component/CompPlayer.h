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

    MOVE,             // ���s
    //DASH,             // ����
    DODGE,            // ���



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

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override;

  // ���N���X��Move�֐������b�v
  void Move(DirectX::XMFLOAT3 direction);

  bool IsRockOn() { return isRockOn; };

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  StateMachine<CompPlayer, ACTION_STATE>& GetActionSM() { return actionStateMachine; }

  DirectX::XMFLOAT3 GetInputVec() { return inputVector; }

  bool InputDodge();

private:
  // ���̓x�N�g���̍X�V
  void UpdateInputVector();

private:
  // �ړ����x
  const float MOVE_SPEED_NO_WEAPON  = 8.0f;
  const float MOVE_SPEED_USE_ITEM  = 6.0f;
  const float MOVE_SPEED_AXE        = 6.0f;
  const float MOVE_SPEED_SWORD      = 4.8f;

  bool isRockOn = false;

  // �v���C���[�̏�
  CONDITION condition = CONDITION::NORMAL;

  std::weak_ptr<CompCharacter> attackTarget;

  // �X�e�B�b�N�̓��͕���
  DirectX::XMFLOAT3 inputVector = {};

  // �s������}�V��
  StateMachine<CompPlayer, ACTION_STATE>  actionStateMachine; // ���C���̍s����S��
};
