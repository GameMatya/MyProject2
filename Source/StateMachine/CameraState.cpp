#include "CameraState.h"
#include "Camera/Camera.h"
#include "GameObject/GameObjectManager.h"
#include "System/Input.h"
#include "Math/OriginMath.h"
#include "imgui.h"

CameraStateFollow::CameraStateFollow(CompCameraPlayer* owner) :State(owner)
{
  ownerObject = owner->GetGameObject();
}

void CameraStateFollow::Enter()
{
  SetAngleFromForward();
}

void CameraStateFollow::Execute(const float& elapsedTime)
{
  // ���b�N�I���Ώۂ������ꍇ
  if (owner->GetPlayer().lock()->IsRockOn() == true) {
    // �X�e�[�g�J��
    owner->GetStateMachine().ChangeState(CompCameraPlayer::CAMERA_STATE::ROCK_ON);
    return;
  }

  // �R���g���[���[�̓��͂����]�s����쐬
  const DirectX::XMMATRIX& Rotation = CreateRotation(elapsedTime);
  ObjectTransform& transform = ownerObject.lock()->transform;

  DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionRotationMatrix(Rotation));

  // ��]�s�񂩂�O�����x�N�g�������o��
  DirectX::XMFLOAT3 forward;
  {
    DirectX::XMVECTOR Front = Rotation.r[2];
    DirectX::XMStoreFloat3(&forward, Front);
  }

  // �����_�̐���
  DirectX::XMFLOAT3 focus;
  {
    DirectX::XMFLOAT3 objectPosition = owner->GetPlayerPos();
    DirectX::XMFLOAT3 vec = owner->GetOldPlayerPos() - objectPosition;

    // �E�����̈ړ�
    DirectX::XMFLOAT3 rightMove = AxisMoveVolume(Rotation.r[0], vec, CompCameraPlayer::CHASE_SPEED_X);

    // ������̈ړ�
    DirectX::XMFLOAT3 upMove = AxisMoveVolume(Rotation.r[1], vec, CompCameraPlayer::CHASE_SPEED_Y);

    // ���s�������̈ړ�
    DirectX::XMFLOAT3 forwardMove = AxisMoveVolume(Rotation.r[2], vec, CompCameraPlayer::CHASE_SPEED_Z);

    // �����_���쐬
    focus = objectPosition + forwardMove + rightMove + upMove;
  }

  // �����_����������Ɉ�苗�����ꂽ�J�����ʒu�����߂�
  transform.position = focus + (forward * -owner->GetDistance());

  Camera::Instance().CollideStage(ownerObject.lock()->GetObjectManager()->GetStageManager(), transform.position, focus);

  // �J�����̎��_�ƒ����_��ݒ�
  Camera::Instance().SetLookAt(transform.position, focus, owner->CalcCameraUp());
}

inline DirectX::XMFLOAT3 CameraStateFollow::AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed)
{
  DirectX::XMFLOAT3 axis = {};
  DirectX::XMStoreFloat3(&axis, Axis);

  float vecDotRight = DirectX::XMVectorGetX(
    DirectX::XMVector3Dot(DirectX::XMVECTOR({ axis.x,axis.y,axis.z }), DirectX::XMVECTOR({ Vec.x,Vec.y,Vec.z })));

  return axis * vecDotRight * MoveSpeed;
}

inline DirectX::XMMATRIX CameraStateFollow::CreateRotation(const float& elapsedTime)
{
  GamePad& gamePad = Input::Instance().GetGamePad();
  float ax = gamePad.GetAxisRX();
  float ay = gamePad.GetAxisRY();

  ObjectTransform& transform = ownerObject.lock()->transform;

  // �J�����̉�]���x
  float speed = ROLL_SPEED * elapsedTime;
  // �X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
  angle.x += ay * speed;
  angle.y += ax * speed;

  // X���̃J������]�𐧌�
  if (angle.x < MIN_ANGLE_X) {
    angle.x = MIN_ANGLE_X;
  }
  if (angle.x > MAX_ANGLE_X) {
    angle.x = MAX_ANGLE_X;
  }
  // Y���̉�]�l��-3.14 ~ 3.14�Ɏ��܂�悤�ɂ���
  if (angle.y < -DirectX::XM_PI)
  {
    angle.y += DirectX::XM_2PI;
  }
  if (angle.y > DirectX::XM_PI)
  {
    angle.y -= DirectX::XM_2PI;
  }

  // �J������]�l����]�s��ɕϊ�
  return DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
}

void CameraStateFollow::SetAngleFromForward()
{
  const DirectX::XMFLOAT3& forward = ownerObject.lock()->transform.forward;

  angle = { -forward.y,atan2f(forward.x,forward.z),0 };
}

CameraStateRockOn::CameraStateRockOn(CompCameraPlayer* owner) :State(owner)
{
  ownerObject = owner->GetGameObject();
}

void CameraStateRockOn::Enter()
{
  targetObject = owner->GetPlayer().lock()->GetAttackTarget().lock()->GetGameObject();
}

void CameraStateRockOn::Execute(const float& elapsedTime)
{
  // ���b�N�I�����������ꂽ�ꍇ
  if (owner->GetPlayer().lock()->IsRockOn() == false) {
    // �X�e�[�g�J��
    owner->GetStateMachine().ChangeState(CompCameraPlayer::CAMERA_STATE::FOLLOW);
    return;
  }

  Camera& camera = Camera::Instance();
  ObjectTransform& transform = ownerObject.lock()->transform;

  // ��]�s�񂩂�O�����x�N�g�������o��
  DirectX::XMFLOAT3 forward = camera.GetForward();

  // �J�����̈ʒu���쐬
  {
    DirectX::XMFLOAT3 playerPosition = owner->GetPlayerPos();
    DirectX::XMFLOAT3 vec = owner->GetOldPlayerPos() - playerPosition;

    // �E�����̈ړ�
    DirectX::XMFLOAT3 rightMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetRight()), vec, CompCameraPlayer::CHASE_SPEED_X);

    // ������̈ړ�
    DirectX::XMFLOAT3 upMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetUp()), vec, CompCameraPlayer::CHASE_SPEED_Y);

    // ���s�������̈ړ�
    DirectX::XMFLOAT3 forwardMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetForward()), vec, CompCameraPlayer::CHASE_SPEED_Z);

    playerPosition += forwardMove + rightMove + upMove;

    // ���S�ʒu����������Ɉ�苗���A���ꂽ�J�����ʒu�����߂�
    transform.position = playerPosition + (forward * -owner->GetDistance());
  }

  // �����_�̍쐬
  DirectX::XMFLOAT3 cameraVec = {};
  {
    CompCharacter* chara = targetObject.lock()->GetComponent<CompCharacter>().get();
    DirectX::XMFLOAT3 targetVec = targetObject.lock()->transform.position + DirectX::XMFLOAT3(0, chara->GetWaistHeight(), 0) - transform.position;
    DirectX::XMFLOAT3 currentVec = owner->GetPlayer().lock()->GetGameObject()->transform.position - transform.position;

    // ���K��
    targetVec = Mathf::Normalize(targetVec);
    currentVec = Mathf::Normalize(currentVec);

    float dot = Mathf::Dot(targetVec, currentVec);

    // ���X�Ƀ^�[�Q�b�g�̕��Ɍ���
    float speed = 1 - fabs(dot);
    cameraVec = Mathf::Lerp(camera.GetForward(), targetVec, pow(speed, RockOnPower));

    // y�����������b�N�I������������
    cameraVec.y = std::lerp(camera.GetForward().y, targetVec.y, pow(speed, ROCKON_POWER_Y));
  }

  ownerObject.lock()->LookAt(transform.position + cameraVec);

  // �J�����̎��_�ƒ����_��ݒ�
  Camera::Instance().SetLookTo(transform.position, cameraVec, owner->CalcCameraUp());
}

inline DirectX::XMFLOAT3 CameraStateRockOn::AxisMoveVolume(const DirectX::XMVECTOR& Axis, const DirectX::XMFLOAT3& Vec, const float& MoveSpeed)
{
  DirectX::XMFLOAT3 axis = {};
  DirectX::XMStoreFloat3(&axis, Axis);

  float vecDotRight = DirectX::XMVectorGetX(
    DirectX::XMVector3Dot(DirectX::XMVECTOR({ axis.x,axis.y,axis.z }), DirectX::XMVECTOR({ Vec.x,Vec.y,Vec.z })));

  return axis * vecDotRight * MoveSpeed;
}
