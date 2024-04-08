#include "HelicopterStates.h"
#include "GameObject/GameObjectManager.h"

void HeliIdle::Enter()
{
  ModelAnimator& animator = owner->GetModel()->animator;
  animator.PlayAnimation(ModelAnimator::BOTTOM, animator.FindAnimationId("Fly"), true);

  // 待機時間をランダム設定
  idleTimer = Mathf::RandomRange(IDLE_TIME_MIN, IDLE_TIME_MAX);
}

void HeliIdle::Execute(const float& elapsedTime)
{
  // タイマー処理 
  idleTimer -= elapsedTime;

  if (idleTimer < 0.0f)
  {
    // 徘徊ステートへ遷移
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::WANDER);
    return;
  }

  // プレイヤーを索敵
  if (owner->GetSupport()->SearchPlayer())
  {
    // 見つかったら追跡ステートへ遷移
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
    return;
  }
}

void HeliWander::Enter()
{
  // 目標地点設定
  owner->GetSupport()->SetRandomTarget();
}

void HeliWander::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  // 目標地点までXZ平面での距離判定
  DirectX::XMFLOAT3 vec = support->GetTargetVec();
  vec.y = 0;

  float distSq = Mathf::Dot(vec);

  // 目標地点についた時
  if (distSq < 0.5f * 0.5f)
  {
    // 待機ステートへ遷移
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::IDLE);
    return;
  }

  // プレイヤーを索敵
  if (support->SearchPlayer())
  {
    // 見つかったら追跡ステートへ遷移
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
    return;
  }

  // 目標地点へ移動
  owner->Move(support->GetTargetDirXZ(), owner->GetMoveSpeed());
  owner->Turn(elapsedTime, support->GetTargetDirXZ(), owner->GetTurnSpeed());

  // アニメーション
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

void HeliPursuit::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  playerVec.y = 0.0f;

  // プレイヤーに近づくと攻撃ステートへ遷移
  float distSq = Mathf::Dot(playerVec);
  if (distSq < PURSUIT_DISTANCE * PURSUIT_DISTANCE)
  {
    // 攻撃ステートへ遷移
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::ATTACK);
  }

  // 目標地点へ移動
  owner->Move(Mathf::Normalize(playerVec), owner->GetMoveSpeed());
  owner->Turn(elapsedTime, Mathf::Normalize(playerVec), owner->GetTurnSpeed());

  // アニメーション
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

HeliAttack::HeliAttack(CompHelicopter* owner) :State(owner)
{
  compBullet = std::make_shared<CompStraightBullet>(
    "./Data/Sprite/Effect/trailEffect2.png",  // 弾の見た目
    0.2f,                                     // トレイルの半径
    Mathf::RandomRange(90, 130), 1.3f,       // 攻撃力、移動速度
    8.0f, 0.1f);                              // 弾の寿命、当たった際の無敵時間
}

void HeliAttack::Enter()
{
  reverseTime = Mathf::RandomRange(REVERSE_TIME_MIN, REVERSE_TIME_MAX);
}

void HeliAttack::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  // 旋回方向の更新
  UpdateRotationDir(elapsedTime);

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // 機体の向きをプレイヤーに合わせる
  owner->Turn(elapsedTime, Mathf::Normalize(playerVecXZ), owner->GetTurnSpeed());

  // 移動処理
  MoveUpdate(elapsedTime, playerVecXZ);

  // 攻撃処理
  AttackUpdate(elapsedTime, playerVec);

  // アニメーション
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

DirectX::XMFLOAT3 HeliAttack::CalcRightDir(const DirectX::XMFLOAT3& vec)
{
  // 外積で右ベクトルを算出
  DirectX::XMFLOAT3 up = { 0,1,0 };
  return Mathf::Cross(up, Mathf::Normalize(vec));
}

void HeliAttack::UpdateRotationDir(const float& elapsedTime)
{
  // 時間経過で旋回方向をランダムに変更
  reverseTimer += elapsedTime;
  if (reverseTimer > reverseTime) {
    isTurnRight = bool(rand() % 2);
    reverseTime = Mathf::RandomRange(REVERSE_TIME_MIN, REVERSE_TIME_MAX);
    reverseTimer = 0.0f;
  }
}

void HeliAttack::MoveUpdate(const float& elapsedTime, const DirectX::XMFLOAT3& playerVecXZ)
{
  float distSq = Mathf::Dot(playerVecXZ);
  float attackRange = ATTACK_DISTANCE * ATTACK_DISTANCE;
  // プレイヤーが近ければ後退する
  if (distSq < (attackRange / 2)) {
    owner->Move(owner->GetGameObject()->transform.forward * -1.0f, owner->GetMoveSpeed());
  }

  // プレイヤーが遠ければ追跡ステートに行く
  else if (distSq > attackRange) {
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
  }

  // プレイヤーの周りを回る
  else {
    if (isTurnRight) {
      owner->Move(CalcRightDir(playerVecXZ), owner->GetMoveSpeed());
    }
    else {
      owner->Move(CalcRightDir(playerVecXZ) * -1.0f, owner->GetMoveSpeed());
    }
  }
}

void HeliAttack::AttackUpdate(const float& elapsedTime, const DirectX::XMFLOAT3& playerVec)
{
  attackCoolTime -= elapsedTime;

  // 攻撃のクールタイムが0以下なら攻撃
  if (attackCoolTime <= 0.0f) {
    ObjectTransform& transform = owner->GetGameObject()->transform;

    // 弾の生成
    GameObjectManager* manager = owner->GetGameObject()->GetObjectManager();
    SharedObject bullet = compBullet->CreateBullet(
      manager,
      COLLIDER_TAGS::PLAYER,
      Mathf::Normalize(playerVec + DirectX::XMFLOAT3(0.0f, owner->GetSupport()->GetPlayerHeight(), 0.0f)));

    // 位置を設定 ( ヘリの少し下 )
    bullet->transform.position = transform.position + DirectX::XMFLOAT3(0, -0.2f, 0);

    // ランダムにクールタイムを設定
    attackCoolTime = Mathf::RandomRange(ATTACK_TIME_MIN, ATTACK_TIME_MAX);
  }
}

void HeliDeath::Enter()
{
  // 重力の影響を有効化
  owner->SetGravityAffect(GRAVITY_AFFECT);
}

void HeliDeath::Execute(const float& elapsedTime)
{
  ObjectTransform& transform = owner->GetGameObject()->transform;

  // 回転処理
  {
    DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), ROTATION_SPEED * elapsedTime);
    transform.AddRotation(quaternion);
  }
}
