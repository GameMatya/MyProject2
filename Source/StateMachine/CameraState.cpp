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
  // ロックオン対象がいた場合
  if (owner->GetPlayer().lock()->IsRockOn() == true) {
    // ステート遷移
    owner->GetStateMachine().ChangeState(CompCameraPlayer::CAMERA_STATE::ROCK_ON);
    return;
  }

  // コントローラーの入力から回転行列を作成
  const DirectX::XMMATRIX& Rotation = CreateRotation(elapsedTime);
  ObjectTransform& transform = ownerObject.lock()->transform;

  DirectX::XMStoreFloat4(&transform.rotation, DirectX::XMQuaternionRotationMatrix(Rotation));

  // 回転行列から前方向ベクトルを取り出す
  DirectX::XMFLOAT3 forward;
  {
    DirectX::XMVECTOR Front = Rotation.r[2];
    DirectX::XMStoreFloat3(&forward, Front);
  }

  // 注視点の生成
  DirectX::XMFLOAT3 focus;
  {
    DirectX::XMFLOAT3 objectPosition = owner->GetPlayerPos();
    DirectX::XMFLOAT3 vec = owner->GetOldPlayerPos() - objectPosition;

    // 右方向の移動
    DirectX::XMFLOAT3 rightMove = AxisMoveVolume(Rotation.r[0], vec, CompCameraPlayer::CHASE_SPEED_X);

    // 上方向の移動
    DirectX::XMFLOAT3 upMove = AxisMoveVolume(Rotation.r[1], vec, CompCameraPlayer::CHASE_SPEED_Y);

    // 奥行き方向の移動
    DirectX::XMFLOAT3 forwardMove = AxisMoveVolume(Rotation.r[2], vec, CompCameraPlayer::CHASE_SPEED_Z);

    // 注視点を作成
    focus = objectPosition + forwardMove + rightMove + upMove;
  }

  // 注視点から後ろ方向に一定距離離れたカメラ位置を求める
  transform.position = focus + (forward * -owner->GetDistance());

  Camera::Instance().CollideStage(ownerObject.lock()->GetObjectManager()->GetStageManager(), transform.position, focus);

  // カメラの視点と注視点を設定
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

  // カメラの回転速度
  float speed = ROLL_SPEED * elapsedTime;
  // スティックの入力値に合わせてX軸とY軸を回転
  angle.x += ay * speed;
  angle.y += ax * speed;

  // X軸のカメラ回転を制限
  if (angle.x < MIN_ANGLE_X) {
    angle.x = MIN_ANGLE_X;
  }
  if (angle.x > MAX_ANGLE_X) {
    angle.x = MAX_ANGLE_X;
  }
  // Y軸の回転値を-3.14 ~ 3.14に収まるようにする
  if (angle.y < -DirectX::XM_PI)
  {
    angle.y += DirectX::XM_2PI;
  }
  if (angle.y > DirectX::XM_PI)
  {
    angle.y -= DirectX::XM_2PI;
  }

  // カメラ回転値を回転行列に変換
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
  // ロックオンが解除された場合
  if (owner->GetPlayer().lock()->IsRockOn() == false) {
    // ステート遷移
    owner->GetStateMachine().ChangeState(CompCameraPlayer::CAMERA_STATE::FOLLOW);
    return;
  }

  Camera& camera = Camera::Instance();
  ObjectTransform& transform = ownerObject.lock()->transform;

  // 回転行列から前方向ベクトルを取り出す
  DirectX::XMFLOAT3 forward = camera.GetForward();

  // カメラの位置を作成
  {
    DirectX::XMFLOAT3 playerPosition = owner->GetPlayerPos();
    DirectX::XMFLOAT3 vec = owner->GetOldPlayerPos() - playerPosition;

    // 右方向の移動
    DirectX::XMFLOAT3 rightMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetRight()), vec, CompCameraPlayer::CHASE_SPEED_X);

    // 上方向の移動
    DirectX::XMFLOAT3 upMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetUp()), vec, CompCameraPlayer::CHASE_SPEED_Y);

    // 奥行き方向の移動
    DirectX::XMFLOAT3 forwardMove = AxisMoveVolume(DirectX::XMLoadFloat3(&camera.GetForward()), vec, CompCameraPlayer::CHASE_SPEED_Z);

    playerPosition += forwardMove + rightMove + upMove;

    // 中心位置から後ろ方向に一定距離、離れたカメラ位置を求める
    transform.position = playerPosition + (forward * -owner->GetDistance());
  }

  // 注視点の作成
  DirectX::XMFLOAT3 cameraVec = {};
  {
    CompCharacter* chara = targetObject.lock()->GetComponent<CompCharacter>().get();
    DirectX::XMFLOAT3 targetVec = targetObject.lock()->transform.position + DirectX::XMFLOAT3(0, chara->GetWaistHeight(), 0) - transform.position;
    DirectX::XMFLOAT3 currentVec = owner->GetPlayer().lock()->GetGameObject()->transform.position - transform.position;

    // 正規化
    targetVec = Mathf::Normalize(targetVec);
    currentVec = Mathf::Normalize(currentVec);

    float dot = Mathf::Dot(targetVec, currentVec);

    // 徐々にターゲットの方に向く
    float speed = 1 - fabs(dot);
    cameraVec = Mathf::Lerp(camera.GetForward(), targetVec, pow(speed, RockOnPower));

    // y成分だけロックオンを強くする
    cameraVec.y = std::lerp(camera.GetForward().y, targetVec.y, pow(speed, ROCKON_POWER_Y));
  }

  ownerObject.lock()->LookAt(transform.position + cameraVec);

  // カメラの視点と注視点を設定
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
