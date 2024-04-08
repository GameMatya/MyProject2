#include "CompCameraPlayer.h"
#include "System/framework.h"
#include "Camera/Camera.h"
#include "StateMachine/CameraState.h"

void CompCameraPlayer::Start()
{
  // ステート追加
  cameraSM.AddState(std::make_shared<CameraStateFollow>(this));
  cameraSM.AddState(std::make_shared<CameraStateRockOn>(this));

  // 初期ステートを設定
  cameraSM.ChangeState(CAMERA_STATE::FOLLOW);

  oldPlayerPosition = player.lock()->GetGameObject()->transform.position;
}

void CompCameraPlayer::Update(const float& elapsedTime)
{
  if (player.expired() == true)return;

  // オブジェクトを滑らかに追いかける
  oldPlayerPosition = Mathf::Lerp(oldPlayerPosition, GetPlayerPos(), CompCameraPlayer::CHASE_SPEED * elapsedTime);
  // 前のフレームの位置を保存
  oldPosition = gameObject.lock()->transform.position;

  UpdateDistance();
  UpdateCameraSlope();

  // ステートマシンの更新
  cameraSM.Update(elapsedTime);

  // UIの更新
  UpdatePlayerUI();
}

void CompCameraPlayer::UpdateDistance()
{
  CompPlayer::MAIN_STATE currentState = player.lock()->GetMainSM().GetCurrentState();

  float nextDistance = DISTANCE;

  // ステートが切り替わった時 
  {
    // 近接攻撃を開始した時
    if (currentState == CompPlayer::MAIN_STATE::SLASH_APPROACH ||
      currentState == CompPlayer::MAIN_STATE::SECOND_APPROACH) {

      nextDistance = DISTANCE_SLASH;
    }
  }

  distance = std::lerp(distance, nextDistance, 0.3f);
}

void CompCameraPlayer::UpdateCameraSlope()
{
  CompPlayer::MAIN_STATE currentState = player.lock()->GetMainSM().GetCurrentState();

  if (player.lock()->GetBoostMode() == true)
  {
    if (currentState == CompPlayer::MAIN_STATE::MOVE) {

      DirectX::XMFLOAT3 velocity = player.lock()->GetVelocity();
      velocity.y = 0.0f;
      float velocityRate = Mathf::Dot(velocity, velocity);
      velocityRate = velocityRate / (player.lock()->GetMaxMoveSpeed() * player.lock()->GetMaxMoveSpeed());
      velocityRate = min(velocityRate, 1.0f);

      slopeApplyRate = std::lerp(slopeApplyRate, SLOPE_RATE_MOVE * velocityRate, 0.1f);
      return;
    }
    if (currentState == CompPlayer::MAIN_STATE::QUICK_BOOST) {
      slopeApplyRate = std::lerp(slopeApplyRate, SLOPE_RATE_BOOST, 0.1f);
      return;
    }
  }
  slopeApplyRate = std::lerp(slopeApplyRate, 0.0f, 0.1f);
}

DirectX::XMFLOAT3 CompCameraPlayer::CalcCameraUp()
{
  if (slopeApplyRate > 0.0f) {
    DirectX::XMFLOAT3 move = gameObject.lock()->transform.position - oldPosition;
    move.y = 0;
    
    // カメラが移動している
    if (Mathf::Dot(move, move) >= 0.0001f) {
      move = Mathf::Normalize(move);

      DirectX::XMFLOAT3 newUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
      newUp += move * slopeApplyRate;
      newUp = Mathf::Normalize(newUp);

      cameraUp = Mathf::Lerp(cameraUp, newUp, 0.5f);
    }

    return cameraUp;
  }

  return DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
}

void CompCameraPlayer::UpdatePlayerUI()
{
  // 攻撃対象が居る場合、UIの位置をロックオン対象のスクリーン座標にする
  if (player.lock()->GetAttackTarget().expired() == false) {
    Camera& camera = Camera::Instance();

    GameObject* enemy = player.lock()->GetAttackTarget().lock().get();
    CompCharacter* compChara = enemy->GetComponent<CompCharacter>().get();

    // スクリーン座標系での座標
    DirectX::XMFLOAT3 position = enemy->transform.position;
    position.y += compChara->GetWaistHeight();
    DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
      DirectX::XMLoadFloat3(&position),
      0.0f,	          //	ビューポート左上X座標
      0.0f,	          //	ビューポート左上Y座標
      SCREEN_WIDTH,	  //	ビューポート幅
      SCREEN_HEIGHT,	//	ビューポート高さ
      0.0f,	// 深度値の最小値
      1.0f,	// 深度値の最大値
      DirectX::XMLoadFloat4x4(&camera.GetProjection()),	//	プロジェクション行列
      DirectX::XMLoadFloat4x4(&camera.GetView()),	//	ビュー行列
      DirectX::XMMatrixIdentity()
    );
    DirectX::XMStoreFloat2(&player.lock()->uiParams.screenPos, ScreenPosition);
  }
  // 攻撃対象が居ない場合、スクリーンの中心にUIを配置
  else {
    player.lock()->uiParams.screenPos = DirectX::XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0f;
  }
}
