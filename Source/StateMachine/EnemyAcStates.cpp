#include "EnemyAcStates.h"

void EnemyAcPursuit::Enter()
{
  reverseTimer = 0.0f;
}

void EnemyAcPursuit::Execute(const float& elapsedTime)
{
  // ���̃X�e�[�g�ɑJ�ڂ��邩����
  StateJudgement();

  // ���E�ړ��̐؂�ւ��^�C�~���O�X�V
  UpdateReverseTimer(elapsedTime);

  // �ړ��x�N�g�����X�V
  UpdateMoveVec();

  // �����_���s��
  owner->RandomAction(
    GUN_PROBABILITY,
    BLADE_PROBABILITY,
    JUMP_PROBABILITY,
    QB_PROBABILITY
  );
}

void EnemyAcPursuit::UpdateReverseTimer(const float& elapsedTime)
{
  reverseTimer -= elapsedTime;

  if (reverseTimer <= 0.0f) {
    reverseTimer = Mathf::RandomRange(REVERSE_TIME_MIN, REVERSE_TIME_MAX);
    isTurnRight = !isTurnRight;
  }
}

void EnemyAcPursuit::UpdateMoveVec()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // �v���C���[�ɑ΂��ĉ������̃x�N�g�������߂�
  DirectX::XMFLOAT3 sideVec = {};
  {
    DirectX::XMFLOAT3 upVec = { 0.0f,1.0f,0.0f };

    // �E�x�N�g��
    sideVec = Mathf::Cross(upVec, playerVecXZ);

    // ���ړ��̏ꍇ�̓x�N�g���𔽓]������
    if (isTurnRight == false) {
      sideVec *= -1.0f;
    }

    // ���������͖������Ă���
    sideVec.y = 0.0f;
  }

  // �v���C���[�Ɍ������x�N�g���ɉ������x�N�g��������
  DirectX::XMFLOAT3 moveVec = playerVecXZ + sideVec * SIDE_MOVE_RATE;
  moveVec = Mathf::Normalize(moveVec);

  owner->SetInputMoveVec(moveVec);
}

void EnemyAcPursuit::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�܂ł̋��� ( 2�� )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // ��苗���ȓ��Ȃ�J��
  if (playerDistanceSq <= BATTLE_RANGE * BATTLE_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::BATTLE_RANGE);
  }
}

void EnemyAcRunAway::Execute(const float& elapsedTime)
{
  UpdateMoveVec();

  StateJudgement();

  // �����_���s��
  owner->RandomAction(
    GUN_PROBABILITY,
    BLADE_PROBABILITY,
    JUMP_PROBABILITY,
    QB_PROBABILITY
  );
}

void EnemyAcRunAway::UpdateMoveVec()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };
  playerVecXZ = Mathf::Normalize(playerVecXZ);

  // �v���C���[�Ɍ������x�N�g���𔽓]
  owner->SetInputMoveVec(-playerVecXZ);
}

void EnemyAcRunAway::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�܂ł̋��� ( 2�� )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // ��苗����藣��Ă���
  if (playerDistanceSq >= BATTLE_RANGE * BATTLE_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::BATTLE_RANGE);
  }
}

void EnemyAcInBattleRange::Enter()
{
  reverseTimer = 0.0f;
}

void EnemyAcInBattleRange::Execute(const float& elapsedTime)
{
  // ���̃X�e�[�g�ɑJ�ڂ��邩����
  StateJudgement();

  // ���E�ړ��̐؂�ւ��^�C�~���O�X�V
  UpdateReverseTimer(elapsedTime);

  // �ړ��x�N�g�����X�V
  UpdateMoveVec();

  // �����_���s��
  owner->RandomAction(
    GUN_PROBABILITY,
    BLADE_PROBABILITY,
    JUMP_PROBABILITY,
    QB_PROBABILITY
  );
}

void EnemyAcInBattleRange::UpdateReverseTimer(const float& elapsedTime)
{
  reverseTimer -= elapsedTime;

  if (reverseTimer <= 0.0f) {
    reverseTimer = Mathf::RandomRange(REVERSE_TIME_MIN, REVERSE_TIME_MAX);
    isTurnRight = !isTurnRight;
  }
}

void EnemyAcInBattleRange::UpdateMoveVec()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // �v���C���[�ɑ΂��ĉ������̃x�N�g�������߂�
  DirectX::XMFLOAT3 sideVec = {};
  {
    DirectX::XMFLOAT3 upVec = { 0.0f,1.0f,0.0f };

    // �E�x�N�g��
    sideVec = Mathf::Cross(upVec, playerVecXZ);

    // ���ړ��̏ꍇ�̓x�N�g���𔽓]������
    if (isTurnRight == false) {
      sideVec *= -1.0f;
    }

    // ���������͖������Ă���
    sideVec.y = 0.0f;
  }

  // �v���C���[�Ɍ������x�N�g���ɉ������x�N�g��������
  DirectX::XMFLOAT3 moveVec = playerVecXZ * FORWARD_MOVE_RATE + sideVec;
  moveVec = Mathf::Normalize(moveVec);

  owner->SetInputMoveVec(Mathf::Normalize(moveVec));
}

void EnemyAcInBattleRange::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�܂ł̋��� ( 2�� )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // ��苗�� ����Ă���
  if (playerDistanceSq >= PURSUIT_RANGE * PURSUIT_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::PURSUIT);
  }

  // ��苗�� �߂�
  if (playerDistanceSq <= RUN_AWAY_RANGE * RUN_AWAY_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::RUN_AWAY);
  }
}

void EnemyAcSlashApproach::Execute(const float& elapsedTime)
{
  // ���̃X�e�[�g�ɑJ�ڂ��邩����
  StateJudgement();

  // �ړ��x�N�g�����X�V
  UpdateMoveVec();

  // �����_���s��
  owner->RandomAction(
    GUN_PROBABILITY,
    BLADE_PROBABILITY,
    JUMP_PROBABILITY,
    QB_PROBABILITY
  );
}

void EnemyAcSlashApproach::UpdateMoveVec()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };
  playerVecXZ = Mathf::Normalize(playerVecXZ);

  // �v���C���[�Ɍ������x�N�g��
  owner->SetInputMoveVec(playerVecXZ);
}

void EnemyAcSlashApproach::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // �v���C���[�܂ł̋��� ( 2�� )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // ��苗�� ����Ă���
  if (playerDistanceSq >= PURSUIT_RANGE * PURSUIT_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::PURSUIT);
  }

  // ��苗�� �߂�
  if (playerDistanceSq <= EXECUTE_RANGE * EXECUTE_RANGE) {
    // �ߐڍU�� ����
    owner->ActionBitStand(CompEnemyAC::ACTION_BIT::LEFT_ATTACK);
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::RUN_AWAY);
  }
}
