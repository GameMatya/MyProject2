#include "CompCharacter.h"
#include "GameObject/GameObjectManager.h"
#include "CompModel.h"
#include "Graphics/EffectManager.h"

#include "imgui.h"
#include <algorithm>

void CompCharacter::Start()
{
  model = gameObject.lock()->GetComponent<CompModel>().get();

  // ModelResource����L�����N�^�[�f�[�^��ǂݍ���
  SettingCharacterDatas();

  // �}�l�[�W���[�Ɏ��g��o�^
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterCharacter(gameObject);
}

void CompCharacter::Update(const float& elapsedTime)
{
  // ���͏����X�V
  UpdateVelocity(elapsedTime);
}

void CompCharacter::OnDestroy()
{
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RemoveCharacter(gameObject);
}

void CompCharacter::Move(DirectX::XMFLOAT3 direction, float maxSpeed)
{
  // �ړ������x�N�g����ݒ�
  moveVecXZ.x = direction.x;
  moveVecXZ.z = direction.z;

  // �ő呬�x�ݒ�
  maxMoveSpeed = maxSpeed;
}

void CompCharacter::AnimationFromVectors(DirectX::XMFLOAT3 moveVec, int baseAnime, bool isLoop, const float& blendSecond)
{
  ObjectTransform& transform = gameObject.lock()->transform;
  ModelAnimator& animator = model->animator;

  // �O��A�j���[�V����
  int zAnime = 0;

  // �O�㔻��
  {
    // �L�����N�^�[�̑O�����Ɠ��͕����œ���
    float zDot = Mathf::Dot(transform.forward, moveVec);

    // ���ς̌��ʂ����Ȃ�O�����A�j���[�V����
    zAnime += (zDot < 0);
  }

  // ���E�A�j���[�V���� ( �O��A�j���[�V�����̕� +2 )
  int xAnime = 2;
  // ���E�̓��ϒl ( �u�����h���Ɏg�p )
  float xDot = 0.0f;

  // ���E����
  {
    // �L�����N�^�[�̉E�����Ɠ��͕����œ���
    xDot = Mathf::Dot(transform.right, moveVec);

    // ���ς̌��ʂ����Ȃ�E�����A�j���[�V����
    xAnime += (xDot < 0);
  }

  // ���ꂼ��̔��茋�ʂƌ��݂̃A�j���[�V�������Ⴆ�΁A���茋�ʂ̃A�j���[�V�������Đ�
  if (animator.GetCurrentAnimationId(animator.ANIM_AREA::BOTTOM) != baseAnime + zAnime)
  {
    animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, baseAnime + zAnime, isLoop, blendSecond);
  }
  if (animator.GetCurrentAnimationId(animator.ANIM_AREA::BOTTOM_SUB) != baseAnime + xAnime)
  {
    animator.PlayAnimationBottomSub(baseAnime + xAnime);
  }

  // �ړ��x�N�g�������E�����ɋ߂��قǁA���E�A�j���[�V�����̃u�����h������������
  animator.SetBottomBlendRate(fabs(xDot));
}

void CompCharacter::Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed)
{
  // 1�t���[��������̐��񑬓x
  float speed = turnSpeed * elapsedTime;

  ObjectTransform& transform = gameObject.lock()->transform;

  DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&directionXZ);
  Direction = DirectX::XMVector3Normalize(Direction);
  DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);

  DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&gameObject.lock()->transform.forward);

  // ��]����C�ӎ����쐬
  DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Forward, Direction);
  if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
  {
    return;
  }

  // �x�N�g�����m�̊p�x�̍������߂�
  float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Forward, Direction));
  // �X�s�[�h�𒲐� ( �������߂��Ȃ�قǒx���Ȃ� )
  speed = min(1.0f - dot, speed);

  // ��]����
  DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
  gameObject.lock()->transform.AddRotation(Turn);
}

void CompCharacter::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
  // ���͂ɗ͂�������
  velocity.x += impulse.x;
  velocity.y += impulse.y;
  velocity.z += impulse.z;
}

void CompCharacter::UpdateVelocity(const float& elapsedTime)
{
  // �o�߃t���[��
  float elapsedFrame = 60.0f * elapsedTime;

  // �������͍X�V����
  UpdateVerticalVelocity(elapsedFrame);

  // �������͍X�V����
  UpdateHorizontalVelocity(elapsedFrame);

  // �����ړ��X�V����
  UpdateVerticalMove(elapsedTime);

  // �����ړ��X�V�X�V
  UpdateHorizontalMove(elapsedTime);
}

// �_���[�W��^����
bool CompCharacter::ApplyDamage(float damage, float invincibleTime)
{
  // ���G����
  if (invincibleTimer > 0)return false;

  // �_���[�W��0�̏ꍇ�͌��N��Ԃ��X�V����K�v���Ȃ�
  if (damage == 0)return false;

  // ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
  if (health <= 0)return false;

  // �_���[�W����
  health -= damage;

  // ���G����
  invincibleTimer = invincibleTime;

  // ���S�ʒm
  if (health <= 0)
  {
    health = 0.0f;
    OnDead();
  }
  // �_���[�W�ʒm
  else
  {
    OnDamaged();
  }

  // ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
  return true;
}

void CompCharacter::DrawImGui()
{
  ImGui::InputFloat("Health", &health);
  ImGui::DragFloat("Move Speed", &moveSpeed);

  ImGui::InputFloat3("Velocity", &velocity.x);
  ImGui::Checkbox("IsGround", &isGround);
}

void CompCharacter::OnDamaged()
{
}

void CompCharacter::UpdateInvincibleTimer(const float& elapsedTime)
{
  if (invincibleTimer > 0.0f)
  {
    invincibleTimer -= elapsedTime;
  }
}

void CompCharacter::SettingCharacterDatas()
{
  const ModelResource::CharacterData& data = model->GetResource()->GetCharacterData();

  // ���m�[�h��ǂݍ���
  {
    if (data.spineNodeId > 0)
      model->animator.SetSplitID(data.spineNodeId);
  }

  // �����o���̋�����ݒ�
  GetGameObject()->SetPushPower(data.pushPower);

  // ���̍�����ݒ�
  waistHeight = data.waistHeight * GetGameObject()->transform.scale.y;
}

void CompCharacter::UpdateVerticalVelocity(const float& elapsedFrame)
{
  // �d�͏���
  velocity.y -= GRAVITY * gravityAffect * elapsedFrame;
  velocity.y = (std::max)(velocity.y, MAX_FALL_SPEED);
}

void CompCharacter::UpdateVerticalMove(const float& elapsedTime)
{
  // ���������̈ړ���
  float my = velocity.y * elapsedTime;

  // �����ŃK�^�K�^�ɂȂ�Ȃ��悤����
  if (isGround && slopeRate < 0.5f && velocity.y < 0) {
    my += -slopeRate * 3;
  }

  float stepOffset = 1.0f;
  {
    // ���C�̊J�n�ʒu�͑�����菭����
    DirectX::XMFLOAT3 start = gameObject.lock()->transform.position;
    start.y += stepOffset;

    // ���C�̏I�_�ʒu�͈ړ���̈ʒu
    DirectX::XMFLOAT3 end = gameObject.lock()->transform.position;
    end.y += my;

    // ���C�L���X�g�ɂ��n�ʔ���
    HitResult hit;
    if (gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, end, &hit))
    {
      // �n�ʂɐڒn���Ă���
      gameObject.lock()->transform.position = hit.position;

      // �X�Η��̌v�Z
      DirectX::XMVECTOR Horizon = DirectX::XMLoadFloat3(&hit.normal);
      Horizon = DirectX::XMVectorSetY(Horizon, 0);
      float horizon = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Horizon));

      // �l���P�ɋ߂��قǕǂɂȂ�
      slopeRate = 1 - (hit.normal.y / (hit.normal.y + horizon));

      // ���n����
      if (!isGround)
      {
        OnLanding();
      }
      isGround = true;
      velocity.y = 0.0f;
    }
    else
    {
      // �󒆂ɕ����Ă���
      gameObject.lock()->transform.position.y += my;
      isGround = false;
    }
  }
}

void CompCharacter::UpdateHorizontalVelocity(const float& elapsedFrame)
{
  // XZ���ʂ̑��͂���������
  float lengthSq = velocity.x * velocity.x + velocity.z * velocity.z;
  if (lengthSq > 0.0f)
  {
    // ���C��
    float friction = this->friction * elapsedFrame;
    // �󒆂ɂ���Ƃ��͖��C�͂����炷
    if (!isGround) friction *= airControl;

    // ���C�ɂ�鉡�����̌�������
    if (lengthSq > friction * friction)
    {
      float length = sqrtf(lengthSq);
      float vx = velocity.x / length;
      float vz = velocity.z / length;

      velocity.x -= vx * friction;
      velocity.z -= vz * friction;
    }
    // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌��������^�[��
    else
    {
      velocity.x = 0.0f;
      velocity.z = 0.0f;
      // �ړ��x�N�g�������Z�b�g
      moveVecXZ = {};
      return;
    }
  }

  lengthSq = velocity.x * velocity.x + velocity.z * velocity.z;
  float moveVecLengthSq = Mathf::Dot(moveVecXZ, moveVecXZ);

  // XZ���ʂ̑��͂���������
  if (lengthSq <= maxMoveSpeed * maxMoveSpeed)
  {
    // �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
    if (moveVecLengthSq > 0.0f)
    {
      // ������
      float acceleration = this->accelerate * elapsedFrame;

      // �ړ��x�N�g���ɂ���������
      velocity.x += acceleration * moveVecXZ.x;
      velocity.z += acceleration * moveVecXZ.z;

      // �ő呬�x����
      float length = sqrtf(lengthSq);
      if (length > maxMoveSpeed)
      {
        velocity.x = (velocity.x / length) * maxMoveSpeed;
        velocity.z = (velocity.z / length) * maxMoveSpeed;
      }
    }
  }
  // �ړ����x���ő�ړ����x���傫��������ړ����͂Ɋ�Â��Č���
  else
  {
    // �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
    if (moveVecLengthSq > 0.0f)
    {
      // ������
      float acceleration = this->accelerate * elapsedFrame;

      // �ړ��x�N�g���ɂ���������
      DirectX::XMFLOAT3 newVelocity = velocity;
      newVelocity.x += acceleration * moveVecXZ.x;
      newVelocity.y = 0;
      newVelocity.z += acceleration * moveVecXZ.z;

      // ������̈ړ��x�N�g�������̈ړ��x�N�g����菬������Δ��f ( �������Ă���Δ��f )
      if (Mathf::Dot(newVelocity, newVelocity) < lengthSq) {
        velocity.x = newVelocity.x;
        velocity.z = newVelocity.z;
      }
    }
  }

  // �ړ��x�N�g�������Z�b�g
  moveVecXZ = {};
}

void CompCharacter::UpdateHorizontalMove(const float& elapsedTime)
{
  // �������͗ʌv�Z
  DirectX::XMFLOAT2 Speed = {
    velocity.x,
    velocity.z
  };
  float velocityLengthXZ = DirectX::XMVectorGetX(DirectX::XMVector2Length(XMLoadFloat2(&Speed)));

  if (velocityLengthXZ > 0.0f)
  {
    // �����ړ���
    float mx = velocity.x * elapsedTime;
    float mz = velocity.z * elapsedTime;

    float stepOffset = 0.5f;

    // ���C�̎n�_�ʒu�ƏI�_�ʒu
    DirectX::XMFLOAT3 start = { gameObject.lock()->transform.position.x,gameObject.lock()->transform.position.y + stepOffset,gameObject.lock()->transform.position.z };
    DirectX::XMFLOAT3 end = { gameObject.lock()->transform.position.x + mx,gameObject.lock()->transform.position.y + stepOffset,gameObject.lock()->transform.position.z + mz };

    // ���C�L���X�g�ɂ��ǔ���
    HitResult hit;
    if (gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, end, &hit))
    {
      // �ړ���̈ʒu����ǂ܂ł̃x�N�g��
      DirectX::XMVECTOR Start = XMLoadFloat3(&start);
      DirectX::XMVECTOR End = XMLoadFloat3(&end);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);

      // �ǂ̖@��
      DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

      // ���˃x�N�g����@���x�N�g���Ɏˉe
      DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

      // �␳�ʒu�̌v�Z
      DirectX::XMFLOAT3 correction;
      DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, End));

      // �␳��̈ʒu���ǂɂ߂荞��ł�����
      HitResult hit2;
      if (!gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, correction, &hit2))
      {
        gameObject.lock()->transform.position.x = correction.x;
        gameObject.lock()->transform.position.z = correction.z;
      }
    }
    else
    {
      gameObject.lock()->transform.position.x += mx;
      gameObject.lock()->transform.position.z += mz;
    }
  }
}
