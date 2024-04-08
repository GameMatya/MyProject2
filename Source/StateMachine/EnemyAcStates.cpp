#include "EnemyAcStates.h"

void EnemyAcPursuit::Enter()
{
  reverseTimer = 0.0f;
}

void EnemyAcPursuit::Execute(const float& elapsedTime)
{
  // 他のステートに遷移するか判定
  StateJudgement();

  // 左右移動の切り替えタイミング更新
  UpdateReverseTimer(elapsedTime);

  // 移動ベクトルを更新
  UpdateMoveVec();

  // ランダム行動
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

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // プレイヤーに対して横方向のベクトルを求める
  DirectX::XMFLOAT3 sideVec = {};
  {
    DirectX::XMFLOAT3 upVec = { 0.0f,1.0f,0.0f };

    // 右ベクトル
    sideVec = Mathf::Cross(upVec, playerVecXZ);

    // 左移動の場合はベクトルを反転させる
    if (isTurnRight == false) {
      sideVec *= -1.0f;
    }

    // 高さ成分は無くしておく
    sideVec.y = 0.0f;
  }

  // プレイヤーに向かうベクトルに横方向ベクトルを合成
  DirectX::XMFLOAT3 moveVec = playerVecXZ + sideVec * SIDE_MOVE_RATE;
  moveVec = Mathf::Normalize(moveVec);

  owner->SetInputMoveVec(moveVec);
}

void EnemyAcPursuit::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // プレイヤーまでの距離 ( 2乗 )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // 一定距離以内なら遷移
  if (playerDistanceSq <= BATTLE_RANGE * BATTLE_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::BATTLE_RANGE);
  }
}

void EnemyAcRunAway::Execute(const float& elapsedTime)
{
  UpdateMoveVec();

  StateJudgement();

  // ランダム行動
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

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };
  playerVecXZ = Mathf::Normalize(playerVecXZ);

  // プレイヤーに向かうベクトルを反転
  owner->SetInputMoveVec(-playerVecXZ);
}

void EnemyAcRunAway::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // プレイヤーまでの距離 ( 2乗 )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // 一定距離より離れている
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
  // 他のステートに遷移するか判定
  StateJudgement();

  // 左右移動の切り替えタイミング更新
  UpdateReverseTimer(elapsedTime);

  // 移動ベクトルを更新
  UpdateMoveVec();

  // ランダム行動
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

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // プレイヤーに対して横方向のベクトルを求める
  DirectX::XMFLOAT3 sideVec = {};
  {
    DirectX::XMFLOAT3 upVec = { 0.0f,1.0f,0.0f };

    // 右ベクトル
    sideVec = Mathf::Cross(upVec, playerVecXZ);

    // 左移動の場合はベクトルを反転させる
    if (isTurnRight == false) {
      sideVec *= -1.0f;
    }

    // 高さ成分は無くしておく
    sideVec.y = 0.0f;
  }

  // プレイヤーに向かうベクトルに横方向ベクトルを合成
  DirectX::XMFLOAT3 moveVec = playerVecXZ * FORWARD_MOVE_RATE + sideVec;
  moveVec = Mathf::Normalize(moveVec);

  owner->SetInputMoveVec(Mathf::Normalize(moveVec));
}

void EnemyAcInBattleRange::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // プレイヤーまでの距離 ( 2乗 )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // 一定距離 離れている
  if (playerDistanceSq >= PURSUIT_RANGE * PURSUIT_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::PURSUIT);
  }

  // 一定距離 近い
  if (playerDistanceSq <= RUN_AWAY_RANGE * RUN_AWAY_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::RUN_AWAY);
  }
}

void EnemyAcSlashApproach::Execute(const float& elapsedTime)
{
  // 他のステートに遷移するか判定
  StateJudgement();

  // 移動ベクトルを更新
  UpdateMoveVec();

  // ランダム行動
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

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };
  playerVecXZ = Mathf::Normalize(playerVecXZ);

  // プレイヤーに向かうベクトル
  owner->SetInputMoveVec(playerVecXZ);
}

void EnemyAcSlashApproach::StateJudgement()
{
  EnemySupport* support = owner->GetSupport();

  // プレイヤーまでの距離 ( 2乗 )
  float playerDistanceSq = Mathf::Dot(support->GetPlayerVec());

  // 一定距離 離れている
  if (playerDistanceSq >= PURSUIT_RANGE * PURSUIT_RANGE) {
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::PURSUIT);
  }

  // 一定距離 近い
  if (playerDistanceSq <= EXECUTE_RANGE * EXECUTE_RANGE) {
    // 近接攻撃 入力
    owner->ActionBitStand(CompEnemyAC::ACTION_BIT::LEFT_ATTACK);
    owner->GetActionSM().ChangeState(CompEnemyAC::ACTION_STATE::RUN_AWAY);
  }
}
