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

  accelerate = 3;

  actionStateMachine.AddState(ACTION_STATE::IDLE, std::make_shared<PlayerIdle>(this));
  actionStateMachine.AddState(ACTION_STATE::MOVE, std::make_shared<PlayerMove>(this));
  actionStateMachine.AddState(ACTION_STATE::DODGE, std::make_shared<PlayerDodge>(this));
  actionStateMachine.AddState(ACTION_STATE::MOVE_WEAPON, std::make_shared<PlayerMoveWeapon>(this));

  actionStateMachine.AddState(ACTION_STATE::ATTACK, std::make_shared<PlayerAttack>(this));
  actionStateMachine.AddState(ACTION_STATE::ATTACK_LOOP, std::make_shared<PlayerAttackLoop>(this));

  // 初期ステートを設定
  actionStateMachine.ChangeState(ACTION_STATE::IDLE);

  // 攻撃派生などを設定
  InitAttackContainer();

  // マネージャーに自身を登録
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);
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
  float currentMoveSpeed = GetMoveSpeed();
  float moveSpeed = 0.0f;

  switch (condition)
  {
  case CompPlayer::CONDITION::NORMAL:
    if (dashFlg == true)
      moveSpeed = MOVE_SPEED_DASH;
    else
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
  CompCharacter::Move(direction, std::lerp(currentMoveSpeed, moveSpeed, 0.75f));
}

void CompPlayer::ResetDashFlg()
{
  dashThumbFlg = false;
  dashFlg = false;
}

bool CompPlayer::InputDodge()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_A, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputDash()
{
  GamePad& gamePad = Input::Instance().GetGamePad();

  // R1ボタンが押されている ならTrue
  if (gamePad.GetButtonDown(GamePad::BTN_RIGHT_SHOULDER, EARLY_INPUT_RECEIPT_FRAME) && dashThumbFlg == false) {
    dashFlg = true;
    return dashFlg;
  }
  else if (dashFlg == true) {
    dashFlg = gamePad.GetButton(GamePad::BTN_RIGHT_SHOULDER);
    return dashFlg;
  }

  // R3ボタンでダッシュ入力していた場合、Trueを返す
  if (dashThumbFlg == true && dashFlg == false) {
    return true;
  }
  else if (gamePad.GetButtonDown(GamePad::BTN_LEFT_THUMB, EARLY_INPUT_RECEIPT_FRAME)) {
    dashThumbFlg = true;
    return true;
  }

  return false;
}

bool CompPlayer::InputWeaponSheathAndDraw()
{
  // アイテム使用中は何もしない
  if (condition == CONDITION::USE_ITEM) return false;

  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  // 納刀中の場合
  if (condition == CONDITION::NORMAL) {
    if (gamePad.GetButtonDown(GamePad::BTN_Y, EARLY_INPUT_RECEIPT_FRAME) == false)return false;

    condition = CONDITION::WEAPON_AXE;
  }
  // 抜刀中の場合
  else {
    if (gamePad.GetButtonDown(GamePad::BTN_X, EARLY_INPUT_RECEIPT_FRAME) == false &&
      gamePad.GetButtonDown(GamePad::BTN_RIGHT_SHOULDER, EARLY_INPUT_RECEIPT_FRAME) == false &&
      gamePad.GetButtonDown(GamePad::BTN_LEFT_THUMB, EARLY_INPUT_RECEIPT_FRAME) == false)return false;

    condition = CONDITION::NORMAL;
  }

  // 抜刀・納刀アニメーション再生  
  int animeId = GetModel()->animator.FindAnimationId("WeaponSheatheAndDraw");
  GetModel()->animator.PlayAnimation(ModelAnimator::ANIM_AREA::UPPER, animeId, false);

  return true;
}

bool CompPlayer::InputAttack()
{
  // 派生先
  auto sequence = GetAttackSequence();

  // Y派生
  if (InputButtonY()) {
    if (sequence[0] != CompPlayer::ATTACK_ACTION::EMPTY) {
      SetAttackAction(sequence[0]);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return true;
    }
  }

  // B派生
  if (InputButtonB()) {
    // ループ攻撃の場合
    if (sequence[1] == CompPlayer::ATTACK_ACTION::AXE_LOOP) {
      SetAttackAction(CompPlayer::ATTACK_ACTION::EMPTY);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK_LOOP);
      return true;
    }
    // 通常の攻撃
    if (sequence[1] != CompPlayer::ATTACK_ACTION::EMPTY) {
      SetAttackAction(sequence[1]);
      GetActionSM().ChangeState(CompPlayer::ACTION_STATE::ATTACK);
      return true;
    }
  }

  return false;
}

bool CompPlayer::InputButtonY()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_Y, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputButtonB()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_B, EARLY_INPUT_RECEIPT_FRAME);
}

bool CompPlayer::InputWeaponTransform()
{
  // 入力情報を取得
  GamePad& gamePad = Input::Instance().GetGamePad();

  return gamePad.GetButtonDown(GamePad::BTN_RIGHT_TRIGGER, EARLY_INPUT_RECEIPT_FRAME);
}

#define SetAttackSequence(src,dstY,dstB)  attackSequence.emplace(src, std::array<ATTACK_ACTION, 2>({dstY, dstB}));
#define SetActionAnime(action,animeName)  attackAnimeNames.emplace(action, animeName);

void CompPlayer::InitAttackContainer()
{
  // 攻撃の派生手順を設定
  {
    SetAttackSequence(ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY);
    SetAttackSequence(ATTACK_ACTION::SHEATH, ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::EMPTY);

#pragma region 斧モード
    // 待機から
    SetAttackSequence(ATTACK_ACTION::AXE_IDLE, ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::AXE_LOOP);

    // Y派生 
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y1, ATTACK_ACTION::COMBO_AXE_Y2, ATTACK_ACTION::AXE_LOOP);
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y2, ATTACK_ACTION::COMBO_AXE_Y3, ATTACK_ACTION::AXE_LOOP);
    SetAttackSequence(ATTACK_ACTION::COMBO_AXE_Y3, ATTACK_ACTION::EMPTY, ATTACK_ACTION::AXE_LOOP);

    SetAttackSequence(ATTACK_ACTION::AXE_LOOP_END, ATTACK_ACTION::EMPTY, ATTACK_ACTION::EMPTY);

#pragma endregion

#pragma region 剣モード
    // 待機から
    SetAttackSequence(ATTACK_ACTION::SWORD_IDLE, ATTACK_ACTION::COMBO_SWORD_Y1, ATTACK_ACTION::COMBO_SWORD_B1);

    // Y派生 
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y1, ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_B1);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B1);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::EMPTY, ATTACK_ACTION::COMBO_SWORD_B1);

    // B派生
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B1, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B2);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B2, ATTACK_ACTION::COMBO_SWORD_Y3, ATTACK_ACTION::COMBO_SWORD_B3);
    SetAttackSequence(ATTACK_ACTION::COMBO_SWORD_B3, ATTACK_ACTION::COMBO_SWORD_Y2, ATTACK_ACTION::COMBO_SWORD_B1);

#pragma endregion
  }

  // アニメーションを各アクションに設定
  {
    // 斧
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y1, "AxeAttackComboA_1");
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y2, "AxeAttackComboA_2");
    SetActionAnime(ATTACK_ACTION::COMBO_AXE_Y3, "AxeAttackComboA_3");

    SetActionAnime(ATTACK_ACTION::AXE_DASH, "AxeAttack_Dash");
    SetActionAnime(ATTACK_ACTION::AXE_LOOP, "AxeAttack_Loop1");
    SetActionAnime(ATTACK_ACTION::AXE_LOOP_END, "AxeAttack_LoopEnd");

    // 剣
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y1, "SwordAttackComboA_1");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y2, "SwordAttackComboA_2");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_Y3, "SwordAttackComboA_3");

    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B1, "SwordAttackComboB_1");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B2, "SwordAttackComboB_2");
    SetActionAnime(ATTACK_ACTION::COMBO_SWORD_B3, "SwordAttackComboB_3");
  }
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
