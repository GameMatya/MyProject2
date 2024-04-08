#pragma once

#include "CompArmoredCore.h"
#include "EnemySupport.h"

class CompEnemyAC :public CompArmoredCore
{
public:
  enum class ACTION_STATE
  {
    PURSUIT,
    RUN_AWAY,
    BATTLE_RANGE,
    SLASH_APPROACH
  };

  enum class ACTION_BIT
  {
    QUICK_BOOST = 1,
    JUMP,
    LEFT_ATTACK,
  };

public:
  void Start()override;

  void Update(const float& elapsedTime)override;

  void DrawImGui()override;

  const char* GetName() const { return "EnemyAC"; };

  bool InputBoostMode() override;
  bool InputQuickBoost()override;
  bool InputJump()override;
  bool InputRise(const float& elapsedTime)override;
  bool InputRightHand() override;
  bool InputLeftHand() override;

  void SetInputMoveVec(const DirectX::XMFLOAT3& inputMove);

  EnemySupport* GetSupport() { return &support; }

  StateMachine<CompEnemyAC, ACTION_STATE>& GetActionSM() { return actionSM; }

  // �r�b�g�𗧂Ă�
  void ActionBitStand(const ACTION_BIT& bit) { actionFlagBit |= (1 << (int)bit); }
  // �r�b�g��Q����
  void ActionBitDrop(const ACTION_BIT& bit) { actionFlagBit &= ~(1 << (int)bit); }
  // �r�b�g�̎擾
  unsigned int ActionBitGet() { return actionFlagBit; }

  // 0~100 �͈̔͂Ń����_���ɍs������
  void RandomAction(const float& gunRate,const float& bladeRate,const float& JumpRate,const float& dushRate);

private:
  // �ˌ����͏���
  void InputShot();

  void OnDead() override;

private:
  EnemySupport support;

  // �ړ����͂��s���X�e�[�g�}�V��
  StateMachine<CompEnemyAC, ACTION_STATE> actionSM;

  // �s���Ǘ��r�b�g
  unsigned int actionFlagBit = 0x00000000;

  // �U���A�W�����v�A�N�C�b�N�u�[�X�g�̍s����������̃N�[���^�C��
  const float ACTION_COOL_TIME_MIN = 0.2f;
  const float ACTION_COOL_TIME_MAX = 2.4f;
  float actionCoolTimer = 0.0f;

  // �ˌ����͎���
  const float SHOOTING_TIME_MIN = 0.5f;
  const float SHOOTING_TIME_MAX = 1.2f;
  float shootingTimer = 0.0f;

};
