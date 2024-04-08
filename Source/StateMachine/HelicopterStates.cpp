#include "HelicopterStates.h"
#include "GameObject/GameObjectManager.h"

void HeliIdle::Enter()
{
  ModelAnimator& animator = owner->GetModel()->animator;
  animator.PlayAnimation(ModelAnimator::BOTTOM, animator.FindAnimationId("Fly"), true);

  // �ҋ@���Ԃ������_���ݒ�
  idleTimer = Mathf::RandomRange(IDLE_TIME_MIN, IDLE_TIME_MAX);
}

void HeliIdle::Execute(const float& elapsedTime)
{
  // �^�C�}�[���� 
  idleTimer -= elapsedTime;

  if (idleTimer < 0.0f)
  {
    // �p�j�X�e�[�g�֑J��
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::WANDER);
    return;
  }

  // �v���C���[�����G
  if (owner->GetSupport()->SearchPlayer())
  {
    // ����������ǐՃX�e�[�g�֑J��
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
    return;
  }
}

void HeliWander::Enter()
{
  // �ڕW�n�_�ݒ�
  owner->GetSupport()->SetRandomTarget();
}

void HeliWander::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  // �ڕW�n�_�܂�XZ���ʂł̋�������
  DirectX::XMFLOAT3 vec = support->GetTargetVec();
  vec.y = 0;

  float distSq = Mathf::Dot(vec);

  // �ڕW�n�_�ɂ�����
  if (distSq < 0.5f * 0.5f)
  {
    // �ҋ@�X�e�[�g�֑J��
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::IDLE);
    return;
  }

  // �v���C���[�����G
  if (support->SearchPlayer())
  {
    // ����������ǐՃX�e�[�g�֑J��
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
    return;
  }

  // �ڕW�n�_�ֈړ�
  owner->Move(support->GetTargetDirXZ(), owner->GetMoveSpeed());
  owner->Turn(elapsedTime, support->GetTargetDirXZ(), owner->GetTurnSpeed());

  // �A�j���[�V����
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

void HeliPursuit::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  playerVec.y = 0.0f;

  // �v���C���[�ɋ߂Â��ƍU���X�e�[�g�֑J��
  float distSq = Mathf::Dot(playerVec);
  if (distSq < PURSUIT_DISTANCE * PURSUIT_DISTANCE)
  {
    // �U���X�e�[�g�֑J��
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::ATTACK);
  }

  // �ڕW�n�_�ֈړ�
  owner->Move(Mathf::Normalize(playerVec), owner->GetMoveSpeed());
  owner->Turn(elapsedTime, Mathf::Normalize(playerVec), owner->GetTurnSpeed());

  // �A�j���[�V����
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

HeliAttack::HeliAttack(CompHelicopter* owner) :State(owner)
{
  compBullet = std::make_shared<CompStraightBullet>(
    "./Data/Sprite/Effect/trailEffect2.png",  // �e�̌�����
    0.2f,                                     // �g���C���̔��a
    Mathf::RandomRange(90, 130), 1.3f,       // �U���́A�ړ����x
    8.0f, 0.1f);                              // �e�̎����A���������ۂ̖��G����
}

void HeliAttack::Enter()
{
  reverseTime = Mathf::RandomRange(REVERSE_TIME_MIN, REVERSE_TIME_MAX);
}

void HeliAttack::Execute(const float& elapsedTime)
{
  EnemySupport* support = owner->GetSupport();

  // ��������̍X�V
  UpdateRotationDir(elapsedTime);

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 playerVec = support->GetPlayerVec();
  DirectX::XMFLOAT3 playerVecXZ = { playerVec.x,0.0f,playerVec.z };

  // �@�̂̌������v���C���[�ɍ��킹��
  owner->Turn(elapsedTime, Mathf::Normalize(playerVecXZ), owner->GetTurnSpeed());

  // �ړ�����
  MoveUpdate(elapsedTime, playerVecXZ);

  // �U������
  AttackUpdate(elapsedTime, playerVec);

  // �A�j���[�V����
  owner->AnimationFromVectors(owner->GetMoveVecXZ(), owner->GetForwardAnimeID(), true);
}

DirectX::XMFLOAT3 HeliAttack::CalcRightDir(const DirectX::XMFLOAT3& vec)
{
  // �O�ςŉE�x�N�g�����Z�o
  DirectX::XMFLOAT3 up = { 0,1,0 };
  return Mathf::Cross(up, Mathf::Normalize(vec));
}

void HeliAttack::UpdateRotationDir(const float& elapsedTime)
{
  // ���Ԍo�߂Ő�������������_���ɕύX
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
  // �v���C���[���߂���Ό�ނ���
  if (distSq < (attackRange / 2)) {
    owner->Move(owner->GetGameObject()->transform.forward * -1.0f, owner->GetMoveSpeed());
  }

  // �v���C���[��������ΒǐՃX�e�[�g�ɍs��
  else if (distSq > attackRange) {
    owner->GetStateMachine()->ChangeState(CompHelicopter::STATE_LIST::PURSUIT);
  }

  // �v���C���[�̎�������
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

  // �U���̃N�[���^�C����0�ȉ��Ȃ�U��
  if (attackCoolTime <= 0.0f) {
    ObjectTransform& transform = owner->GetGameObject()->transform;

    // �e�̐���
    GameObjectManager* manager = owner->GetGameObject()->GetObjectManager();
    SharedObject bullet = compBullet->CreateBullet(
      manager,
      COLLIDER_TAGS::PLAYER,
      Mathf::Normalize(playerVec + DirectX::XMFLOAT3(0.0f, owner->GetSupport()->GetPlayerHeight(), 0.0f)));

    // �ʒu��ݒ� ( �w���̏����� )
    bullet->transform.position = transform.position + DirectX::XMFLOAT3(0, -0.2f, 0);

    // �����_���ɃN�[���^�C����ݒ�
    attackCoolTime = Mathf::RandomRange(ATTACK_TIME_MIN, ATTACK_TIME_MAX);
  }
}

void HeliDeath::Enter()
{
  // �d�͂̉e����L����
  owner->SetGravityAffect(GRAVITY_AFFECT);
}

void HeliDeath::Execute(const float& elapsedTime)
{
  ObjectTransform& transform = owner->GetGameObject()->transform;

  // ��]����
  {
    DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), ROTATION_SPEED * elapsedTime);
    transform.AddRotation(quaternion);
  }
}
