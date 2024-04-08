#include "CompPlayer.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "Scenes/SceneGame.h"

#include "CompGun.h"
#include "SceneSystem/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"

#include "imgui.h"
#include <string>

#define INPUT_ACCEPT_FRAME (10)

void CompPlayer::Start()
{
  // 基底クラスのStart関数
  CompArmoredCore::Start();

  // マネージャーに自身を登録
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);

  // パラメーター設定
  moveSpeed = 5.0f;
  boostSpeed = 16.5f;
  jumpSpeed = 15.0f;
  fallSpeed = -15.0f;
  turnSpeed = DirectX::XMConvertToRadians(400);
  MaxHealth = 7400;
  health = MaxHealth;
}

void CompPlayer::Update(const float& elapsedTime)
{
  // ゲームの待機時間中は処理しない
  if (SceneGame::Instance()->StandbyComplete()) {

    // 攻撃対象を探す
    if (isRockOn == false) {
      SearchTargetEnemy();
    }
    else {
      // ロックオンターゲットの生存確認
      if (attackTarget.expired()) {
        isRockOn = false;
      }
    }

    // 移動入力の更新
    UpdateInputMove();

    // ロックオン入力処理
    InputRockOn();

    InputReload();

    // ステートマシンの更新
    mainStateMachine.Update(elapsedTime);
    armRightSM.Update(elapsedTime);
    armLeftSM.Update(elapsedTime);
  }

  // 無敵時間更新
  UpdateInvincibleTimer(elapsedTime);

  // 速力処理更新
  UpdateVelocity(elapsedTime);

  // スラスター処理更新
  thrusterManager.Update(elapsedTime, isGround);

  // UI用のパラメーターを更新
  UpdateUiParameter();
}

void CompPlayer::DrawImGui()
{
  CompArmoredCore::DrawImGui();
  ImGui::Separator();

  {
    ModelAnimator& animator = GetModel()->animator;
    float rate = -1.0f;

    if (animator.IsPlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM))
      rate = animator.GetAnimationRate(ModelAnimator::ANIM_AREA::BOTTOM);

    ImGui::InputFloat("AnimeRate", &rate);
  }

  ImGui::InputFloat("Slope", &slopeRate);
  ImGui::InputFloat("Accele", &accelerate);
}

void CompPlayer::SearchTargetEnemy()
{
  // 攻撃する敵を探す
  const Camera& camera = Camera::Instance();

  // 最小値の敵を探す
  float minValue = FLT_MAX;

  // カメラの前ベクトル
  DirectX::XMVECTOR CameraVec = DirectX::XMLoadFloat3(&camera.GetForward());

  // 全ての敵をチェック
  WeakObject result;
  for (auto& enemy : gameObject.lock()->GetObjectManager()->GetCharacterManager().GetEnemies()) {
    if (enemy.lock()->GetComponent<CompCharacter>()->GetHealth() <= 0) continue;

    // カメラから敵のベクトル
    DirectX::XMVECTOR EnemyVec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&enemy.lock()->transform.position), DirectX::XMLoadFloat3(&camera.GetEye()));

    // カメラと敵の距離 ( 2乗 )
    float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(EnemyVec));
    // 索敵範囲外なら無視
    if (distSq > SEARCH_DISTANCE * SEARCH_DISTANCE)continue;

    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(EnemyVec), DirectX::XMVector3Normalize(CameraVec)));
    // 内積の結果が負なら無視 ( 画面に映っているかの確認 )
    if (dot <= SEARCH_RANGE) continue;

    // value = 画面中央からの距離 * カメラ位置と敵の距離
    float value = (1 - dot) * distSq;
    if (value < minValue) {
      // 最小値の更新
      minValue = value;

      result = enemy;
    }
  }
  attackTarget = result;
}

bool CompPlayer::InputBoostMode()
{
  if (Input::Instance().GetGamePad().GetButtonDown(GamePad::BTN_B)) {
    ActiveBoostMode();
    return true;
  }
  return false;
}

bool CompPlayer::InputQuickBoost()
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (Input::Instance().GetGamePad().GetButtonDown(GamePad::BTN_X, INPUT_ACCEPT_FRAME) == false)return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_QB);
  return true;
}

bool CompPlayer::InputJump()
{
  return Input::Instance().GetGamePad().GetButtonDown(GamePad::BTN_A);
}

bool CompPlayer::InputRise(const float& elapsedTime)
{
  if (thrusterManager.IsOverHeat() == true)return false;
  if (Input::Instance().GetGamePad().GetButton(GamePad::BTN_A) == false)return false;

  thrusterManager.QuantitySubtract(THRUSTER_COST_RISE * elapsedTime);
  return true;
}

// 攻撃入力処理
bool CompPlayer::InputRightHand()
{
  // R2ボタン押下で射撃
  GamePad& gamePad = Input::Instance().GetGamePad();
  Mouse& mouse = Input::Instance().GetMouse();

#ifdef _DEBUG
  if (gamePad.GetButton(GamePad::BTN_RIGHT_TRIGGER) ||
    gamePad.GetButton(GamePad::BTN_LEFT_SHOULDER)) {
#else
  if (gamePad.GetButton(GamePad::BTN_RIGHT_TRIGGER) ||
    mouse.GetButton(Mouse::BTN_RIGHT)
    ) {
#endif
    if (armRightSM.GetCurrentState() != ARM_STATE::SHOT)
      armRightSM.ChangeState(ARM_STATE::SHOT);

    return true;
  }
  else
    return false;
  }

bool CompPlayer::InputLeftHand()
{
  GamePad& gamePad = Input::Instance().GetGamePad();
  Mouse& mouse = Input::Instance().GetMouse();

  // 左手武器が銃だった場合
  if (leftWeapon != nullptr && leftWeapon->IsSword() == false) {
#ifdef _DEBUG
    if (gamePad.GetButton(GamePad::BTN_LEFT_TRIGGER) == false && mouse.GetButton(Mouse::BTN_RIGHT) == false)
      return false;
#else
    if (gamePad.GetButton(GamePad::BTN_LEFT_TRIGGER) == false && mouse.GetButton(Mouse::BTN_LEFT) == false)
      return false;
#endif

    return true;
}

  // 左手武器が近接武器だった場合
  else {
    if (thrusterManager.IsOverHeat() == true)return false;
    if (leftWeapon->CanExecute() == false)return false;

#ifdef _DEBUG
    if (gamePad.GetButtonDown(GamePad::BTN_LEFT_TRIGGER, INPUT_ACCEPT_FRAME) == false && mouse.GetButtonDown(Mouse::BTN_RIGHT, INPUT_ACCEPT_FRAME) == false)
      return false;
#else
    if (gamePad.GetButtonDown(GamePad::BTN_LEFT_TRIGGER, INPUT_ACCEPT_FRAME) == false && mouse.GetButtonDown(Mouse::BTN_LEFT, INPUT_ACCEPT_FRAME) == false)
      return false;
#endif

    ActiveBoostMode();
  }

  return true;
}

// ロックオン入力処理
void CompPlayer::InputRockOn()
{
  GamePad& gamePad = Input::Instance().GetGamePad();
  Mouse& mouse = Input::Instance().GetMouse();

  // マウスのホイールクリック or Rスティック押し込みでロックオンカメラ
  if (gamePad.GetButtonDown(GamePad::BTN_RIGHT_THUMB) || mouse.GetButtonDown(Mouse::BTN_MIDDLE)) {

    // ロックオン状態でなければ
    if (isRockOn == false)
    {
      // 敵がいなかったらFALSE
      if (attackTarget.expired())return;

      // 攻撃対象をロックオン対象に昇格
      isRockOn = true;
    }
    // すでにロックオン状態の場合
    else
    {
      isRockOn = false;
    }
  }
}

void CompPlayer::InputReload()
{
  // 近接攻撃中はリロード不可
  if (mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_EXECUTE)return;

  GamePad& gamePad = Input::Instance().GetGamePad();

  // Yボタン( Rキー )を押しながら
  if (gamePad.GetButton(GamePad::BTN_Y) == false) return;

  Mouse& mouse = Input::Instance().GetMouse();

  // RT ( 右クリック )
  if (mouse.GetButtonDown(Mouse::BTN_RIGHT) || gamePad.GetButtonDown(GamePad::BTN_RIGHT_TRIGGER)) {
    // 射撃中はリロード不可
    if (armRightSM.GetCurrentState() == ARM_STATE::SHOT)return;

    CompGun* gun = dynamic_cast<CompGun*>(rightWeapon);
    if (gun != nullptr && gun->IsReload() == false) {
      gun->Reload();
    }
  }

  // LT ( 左クリック )
  if (mouse.GetButtonDown(Mouse::BTN_LEFT) || gamePad.GetButtonDown(GamePad::BTN_LEFT_TRIGGER)) {
    // 射撃中はリロード不可
    if (armLeftSM.GetCurrentState() == ARM_STATE::SHOT)return;

    CompGun* gun = dynamic_cast<CompGun*>(leftWeapon);
    if (gun != nullptr && gun->IsReload() == false) {
      gun->Reload();
    }
  }
}

void CompPlayer::UpdateInputMove()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();
  float ax = gamePad.GetAxisLX();
  float ay = gamePad.GetAxisLY();

  // 入力が無ければ何もしない
  if (ax == 0.0f && ay == 0.0f) {
    inputVector = { 0.0f,0.0f,0.0f };
    return;
  }

#if(0)
  DirectX::XMFLOAT3 rightVec = {};
  DirectX::XMFLOAT3 forwardVec = {};

  // ロックオン時は敵に向かうベクトルを軸にする
  if (isRockOn == true) {
    forwardVec = GetAttackTargetVec();

    // 外積で右ベクトルを求める
    DirectX::XMFLOAT3 up = { 0,1,0 };
    rightVec = Mathf::Cross(up, forwardVec);
  }
  // カメラ方向とスティックの入力値によって進行方向を計算する
  else {
    Camera& camera = Camera::Instance();
    rightVec = camera.GetRight();
    forwardVec = camera.GetForward();
}
#else
  // カメラ方向とスティックの入力値によって進行方向を計算する
  Camera& camera = Camera::Instance();
  const DirectX::XMFLOAT3& rightVec = camera.GetRight();
  const DirectX::XMFLOAT3& forwardVec = camera.GetForward();

#endif

  // 移動ベクトルはXZ平面に水平なベクトルになるようにする
  // 右方向ベクトルをXZ単位ベクトルに変換
  DirectX::XMFLOAT2 rightXZ = { rightVec.x,rightVec.z };
  rightXZ = Mathf::Normalize(rightXZ);

  // 前方向ベクトルをXZ単位ベクトルに変換
  DirectX::XMFLOAT2 forwardXZ = { forwardVec.x,forwardVec.z };
  forwardXZ = Mathf::Normalize(forwardXZ);

  // スティックの水平入力値を右方向に反映し、
  // スティックの垂直入力値を前方向に反映し、
  // 進行ベクトルを計算する
  inputVector.x = forwardXZ.x * ay + rightXZ.x * ax;
  inputVector.y = 0;
  inputVector.z = forwardXZ.y * ay + rightXZ.y * ax;
}

void CompPlayer::UpdateUiParameter()
{
  // スラスター容量
  uiParams.thrusterQuantity = thrusterManager.GetThrusterQuantity() / ThrusterManager::THRUSTER_CAPACITY;

  // UIの見栄えの為、10倍する
  uiParams.elevation = gameObject.lock()->transform.position.y + ELEVATION_OFFSET * 10.0f;
  Mathf::Normalize(velocity, uiParams.moveSpeed);
  uiParams.moveSpeed *= 10.0f;

  // レティクルの位置
  if (attackTarget.expired() == false) {
    uiParams.isRockOn = isRockOn;
    uiParams.isTarget = !isRockOn;
  }
  else {
    uiParams.isRockOn = false;
    uiParams.isTarget = false;
  }

  uiParams.healthRate = health / MaxHealth;
  // ダメージを受けた際の赤ゲージの挙動
  uiParams.damage = std::lerp(uiParams.damage, uiParams.healthRate, 0.1f);
}