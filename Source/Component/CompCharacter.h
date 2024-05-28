#pragma once

#include "Component.h"
#include "CompModel.h"
#include "Graphics/Effect.h"
#include "Math/OriginMath.h"

// �n���̏d��
#define GRAVITY (0.98f)
// �ő嗎�����x
#define MAX_FALL_SPEED (-30.0f)

class CompCharacter :public Component
{
public:
  CompCharacter() {};

  virtual void Start()override;

  virtual void Update(const float& elapsedTime) override;

  // GUI�`��
  virtual void DrawImGui() override;

  void OnDestroy()override;

  // ���O�擾
  virtual const char* GetName() const override { return "Character"; }

  // �_���[�W��^����
  bool ApplyDamage(float damage, float invincibleTime);

  // �ړ�����
  void Move(DirectX::XMFLOAT3 direction, float maxSpeed);

  // ���񏈗�
  void Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed);

  // �����x�N�g���ƈړ��x�N�g������A�j���[�V������ω�������
  // �� Enum�̏��Ԃ͑O,��,�E,���Ƃ���
  void AnimationFromVectors(DirectX::XMFLOAT3 moveVec, int baseAnime, bool isLoop, const float& blendSecond = 0.2f);

  // �Ռ���^����
  void AddImpulse(const DirectX::XMFLOAT3& impulse);

#pragma region �Q�b�^�[�E�Z�b�^�[
  CompModel*    GetModel() { return model; }
  const float&  GetMoveSpeed()  const { return moveSpeed; }
  const float&  GetMaxMoveSpeed()  const { return maxMoveSpeed; }
  const float&  GetJumpSpeed()  const { return jumpSpeed; }
  const float&  GetTurnSpeed()  const { return turnSpeed; }
  const bool&   GetIsGround()   const { return isGround; }
  const DirectX::XMFLOAT3& GetMoveVecXZ() const { return moveVecXZ; }
  const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }
  const float& GetWaistHeight() const { return waistHeight; }
  const float& GetHealth() const { return health; }

  void SetVelocity(const DirectX::XMFLOAT3& vec) { velocity = vec; }
  void SetAccelerate(const float& accele) { accelerate = accele; }
  void SetGravityAffect(const float& affect) { gravityAffect = affect; }

#pragma endregion

protected:
  // ���͏����X�V
  void UpdateVelocity(const float& elapsedTime);

  // ���n�������ɌĂ΂��
  virtual void OnLanding() {}

  // �_���[�W���󂯂����ɌĂ΂��
  virtual void OnDamaged();

  // ���G���ԍX�V
  void UpdateInvincibleTimer(const float& elapsedTime);

  // ���S�������ɌĂ΂��
  virtual void OnDead() {}

private:
  // ModelResource�̃L�����N�^�[�f�[�^��ǂݍ��݁A�ݒ肵�Ă���
  void SettingCharacterDatas();

  // �������͍X�V����
  void UpdateVerticalVelocity(const float& elapsedFrame);
  // �����ړ��X�V����
  void UpdateVerticalMove(const float& elapsedTime);

  // �������͍X�V����
  void UpdateHorizontalVelocity(const float& elapsedFrame);
  // �����ړ��X�V����
  void UpdateHorizontalMove(const float& elapsedTime);

protected:
  CompModel* model = nullptr;

  // �̗�
  float MaxHealth = 1;
  float health = MaxHealth;
  // ���G����
  float invincibleTimer = 0.3f;

  // �d�͂̉e���x
  float gravityAffect = 1.0f;
  // ���C ( �����p )
  float friction = 0.5f;
  // �󒆐���
  float airControl = 0.3f;

  // �X�Η�
  float slopeRate = 1.0f;
  // ���܂ł̍���
  float waistHeight = 0.0f;

  // �ڒn���Ă��邩
  bool isGround = false;

#pragma region �ړ��n�p�����[�^
  // ���s���x
  float moveSpeed = 5.0f;
  // ������
  float accelerate = 1;
  // �ő�ړ����x
  float maxMoveSpeed = 5;
  // ���񑬓x
  float turnSpeed = DirectX::XMConvertToRadians(300);
  // �W�����v�̑��x
  float jumpSpeed = 1.0f;
  // ���n�X�e�[�g�֑J�ڂ��闎�����x
  float fallSpeed = -1.0f;

  // �ړ������x�N�g�� (XZ���ʂ̂�)
  DirectX::XMFLOAT3 moveVecXZ = {};
  // �ړ��l
  DirectX::XMFLOAT3 velocity = {};

#pragma endregion

};
