#pragma once

#include "CompCharacter.h"
#include "Graphics/TrailEffect.h"
#include "StateMachine/StateMachine.h"

class CompPlayer :public CompCharacter
{
public:
  // ���C���̍s��
  enum class MAIN_STATE
  {
    IDLE,             // �ҋ@

    MOVE,             // ���s
    QUICK_BOOST,      // �}����
    DECELERATION,     // ����

    JUMP,             // �W�����v
    AERIAL,           // ��
    LANDING,          // ���n

    SLASH_APPROACH,   // �ߐڍU������
    SLASH_EXECUTE,    // �ߐڍU�����s

    SECOND_APPROACH,  // �ߐڍU������ ( 2��� )
    SECOND_EXECUTE,   // �ߐڍU�����s ( 2��� )

    DEATH,            // ���S
    DAMAGE,           // ��_���[�W

    MAX
  };

public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime) override;

  void DrawImGui()override; 
  
  bool IsRockOn() { return isRockOn; };

  std::weak_ptr<CompCharacter> GetAttackTarget() { return attackTarget; };

  StateMachine<CompCharacter, MAIN_STATE>& GetMainSM() { return mainStateMachine; }

private:
  // �ړ����͂̍X�V
  void UpdateInputMove();

private:
  bool isRockOn = false;

  std::weak_ptr<CompCharacter> attackTarget;

  DirectX::XMFLOAT3 inputVector = {};

  // �s������}�V��
  StateMachine<CompCharacter, MAIN_STATE>  mainStateMachine; // ���C���̍s����S��
};
