#include "CompPlayer.h"
#include "System/Input.h"
#include "Camera/Camera.h"
#include "Scenes/SceneGame.h"

#include "SceneSystem/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "GameObject/CharacterManager.h"

#include "imgui.h"
#include <string>

void CompPlayer::Start()
{
  CompCharacter::Start();

  // マネージャーに自身を登録
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);
}

void CompPlayer::Update(const float& elapsedTime)
{
  // 移動入力の更新
  UpdateInputMove();

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
