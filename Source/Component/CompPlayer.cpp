#include "CompPlayer.h"
#include "Camera/Camera.h"

#include "System/Input.h"
#include "Scenes/SceneGame.h"
#include "SceneSystem/Scene.h"
#include "StateMachine/PlayerState.h"

#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"

#include "imgui.h"

void CompPlayer::Start()
{
  CompCharacter::Start();

  // マネージャーに自身を登録
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);

  actionStateMachine.AddState(std::make_shared<PlayerIdle>(this));
  actionStateMachine.AddState(std::make_shared<PlayerMove>(this));
  actionStateMachine.AddState(std::make_shared<PlayerDodge>(this));

  // 初期ステートを設定
  actionStateMachine.ChangeState(ACTION_STATE::IDLE);
}

void CompPlayer::Update(const float& elapsedTime)
{
  // 移動入力の更新
  UpdateInputVector();

  // 各行動の更新
  actionStateMachine.Update(elapsedTime);

  // 無敵時間更新
  UpdateInvincibleTimer(elapsedTime);

  // 速力処理更新
  UpdateVelocity(elapsedTime);
}

void CompPlayer::DrawImGui()
{
  CompCharacter::DrawImGui();
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

void CompPlayer::Move(DirectX::XMFLOAT3 direction)
{
  float moveSpeed = 0.0f;

  switch (condition)
  {
  case CompPlayer::CONDITION::NORMAL:
    moveSpeed = MOVE_SPEED_NO_WEAPON;
    break;
  case CompPlayer::CONDITION::USE_ITEM:
    moveSpeed = MOVE_SPEED_USE_ITEM;
    break;
  case CompPlayer::CONDITION::WEAPON_AXE:
    moveSpeed = MOVE_SPEED_AXE;
    break;
  case CompPlayer::CONDITION::WEAPON_SWORD:
    moveSpeed = MOVE_SPEED_SWORD;
    break;
  default:
    assert(!"condition error");
    break;
  }

  // 基底クラスのMove関数を呼び出す
  CompCharacter::Move(direction, moveSpeed);
}

bool CompPlayer::InputDodge()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_A);
}

void CompPlayer::UpdateInputVector()
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

  // カメラ方向とスティックの入力値によって進行方向を計算する
  Camera& camera = Camera::Instance();
  const DirectX::XMFLOAT3& rightVec = camera.GetRight();
  const DirectX::XMFLOAT3& forwardVec = camera.GetForward();

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
