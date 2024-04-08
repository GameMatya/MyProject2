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
  // ���N���X��Start�֐�
  CompArmoredCore::Start();

  // �}�l�[�W���[�Ɏ��g��o�^
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterPlayer(gameObject);

  // �p�����[�^�[�ݒ�
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
  // �Q�[���̑ҋ@���Ԓ��͏������Ȃ�
  if (SceneGame::Instance()->StandbyComplete()) {

    // �U���Ώۂ�T��
    if (isRockOn == false) {
      SearchTargetEnemy();
    }
    else {
      // ���b�N�I���^�[�Q�b�g�̐����m�F
      if (attackTarget.expired()) {
        isRockOn = false;
      }
    }

    // �ړ����͂̍X�V
    UpdateInputMove();

    // ���b�N�I�����͏���
    InputRockOn();

    InputReload();

    // �X�e�[�g�}�V���̍X�V
    mainStateMachine.Update(elapsedTime);
    armRightSM.Update(elapsedTime);
    armLeftSM.Update(elapsedTime);
  }

  // ���G���ԍX�V
  UpdateInvincibleTimer(elapsedTime);

  // ���͏����X�V
  UpdateVelocity(elapsedTime);

  // �X���X�^�[�����X�V
  thrusterManager.Update(elapsedTime, isGround);

  // UI�p�̃p�����[�^�[���X�V
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
  // �U������G��T��
  const Camera& camera = Camera::Instance();

  // �ŏ��l�̓G��T��
  float minValue = FLT_MAX;

  // �J�����̑O�x�N�g��
  DirectX::XMVECTOR CameraVec = DirectX::XMLoadFloat3(&camera.GetForward());

  // �S�Ă̓G���`�F�b�N
  WeakObject result;
  for (auto& enemy : gameObject.lock()->GetObjectManager()->GetCharacterManager().GetEnemies()) {
    if (enemy.lock()->GetComponent<CompCharacter>()->GetHealth() <= 0) continue;

    // �J��������G�̃x�N�g��
    DirectX::XMVECTOR EnemyVec = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&enemy.lock()->transform.position), DirectX::XMLoadFloat3(&camera.GetEye()));

    // �J�����ƓG�̋��� ( 2�� )
    float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(EnemyVec));
    // ���G�͈͊O�Ȃ疳��
    if (distSq > SEARCH_DISTANCE * SEARCH_DISTANCE)continue;

    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(EnemyVec), DirectX::XMVector3Normalize(CameraVec)));
    // ���ς̌��ʂ����Ȃ疳�� ( ��ʂɉf���Ă��邩�̊m�F )
    if (dot <= SEARCH_RANGE) continue;

    // value = ��ʒ�������̋��� * �J�����ʒu�ƓG�̋���
    float value = (1 - dot) * distSq;
    if (value < minValue) {
      // �ŏ��l�̍X�V
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

// �U�����͏���
bool CompPlayer::InputRightHand()
{
  // R2�{�^�������Ŏˌ�
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

  // ���蕐�킪�e�������ꍇ
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

  // ���蕐�킪�ߐڕ��킾�����ꍇ
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

// ���b�N�I�����͏���
void CompPlayer::InputRockOn()
{
  GamePad& gamePad = Input::Instance().GetGamePad();
  Mouse& mouse = Input::Instance().GetMouse();

  // �}�E�X�̃z�C�[���N���b�N or R�X�e�B�b�N�������݂Ń��b�N�I���J����
  if (gamePad.GetButtonDown(GamePad::BTN_RIGHT_THUMB) || mouse.GetButtonDown(Mouse::BTN_MIDDLE)) {

    // ���b�N�I����ԂłȂ����
    if (isRockOn == false)
    {
      // �G�����Ȃ�������FALSE
      if (attackTarget.expired())return;

      // �U���Ώۂ����b�N�I���Ώۂɏ��i
      isRockOn = true;
    }
    // ���łɃ��b�N�I����Ԃ̏ꍇ
    else
    {
      isRockOn = false;
    }
  }
}

void CompPlayer::InputReload()
{
  // �ߐڍU�����̓����[�h�s��
  if (mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_APPROACH ||
    mainStateMachine.GetCurrentState() == MAIN_STATE::SLASH_EXECUTE)return;

  GamePad& gamePad = Input::Instance().GetGamePad();

  // Y�{�^��( R�L�[ )�������Ȃ���
  if (gamePad.GetButton(GamePad::BTN_Y) == false) return;

  Mouse& mouse = Input::Instance().GetMouse();

  // RT ( �E�N���b�N )
  if (mouse.GetButtonDown(Mouse::BTN_RIGHT) || gamePad.GetButtonDown(GamePad::BTN_RIGHT_TRIGGER)) {
    // �ˌ����̓����[�h�s��
    if (armRightSM.GetCurrentState() == ARM_STATE::SHOT)return;

    CompGun* gun = dynamic_cast<CompGun*>(rightWeapon);
    if (gun != nullptr && gun->IsReload() == false) {
      gun->Reload();
    }
  }

  // LT ( ���N���b�N )
  if (mouse.GetButtonDown(Mouse::BTN_LEFT) || gamePad.GetButtonDown(GamePad::BTN_LEFT_TRIGGER)) {
    // �ˌ����̓����[�h�s��
    if (armLeftSM.GetCurrentState() == ARM_STATE::SHOT)return;

    CompGun* gun = dynamic_cast<CompGun*>(leftWeapon);
    if (gun != nullptr && gun->IsReload() == false) {
      gun->Reload();
    }
  }
}

void CompPlayer::UpdateInputMove()
{
  // ���͏����擾
  GamePad& gamePad = Input::Instance().GetGamePad();
  float ax = gamePad.GetAxisLX();
  float ay = gamePad.GetAxisLY();

  // ���͂�������Ή������Ȃ�
  if (ax == 0.0f && ay == 0.0f) {
    inputVector = { 0.0f,0.0f,0.0f };
    return;
  }

#if(0)
  DirectX::XMFLOAT3 rightVec = {};
  DirectX::XMFLOAT3 forwardVec = {};

  // ���b�N�I�����͓G�Ɍ������x�N�g�������ɂ���
  if (isRockOn == true) {
    forwardVec = GetAttackTargetVec();

    // �O�ςŉE�x�N�g�������߂�
    DirectX::XMFLOAT3 up = { 0,1,0 };
    rightVec = Mathf::Cross(up, forwardVec);
  }
  // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
  else {
    Camera& camera = Camera::Instance();
    rightVec = camera.GetRight();
    forwardVec = camera.GetForward();
}
#else
  // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
  Camera& camera = Camera::Instance();
  const DirectX::XMFLOAT3& rightVec = camera.GetRight();
  const DirectX::XMFLOAT3& forwardVec = camera.GetForward();

#endif

  // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���
  // �E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
  DirectX::XMFLOAT2 rightXZ = { rightVec.x,rightVec.z };
  rightXZ = Mathf::Normalize(rightXZ);

  // �O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
  DirectX::XMFLOAT2 forwardXZ = { forwardVec.x,forwardVec.z };
  forwardXZ = Mathf::Normalize(forwardXZ);

  // �X�e�B�b�N�̐������͒l���E�����ɔ��f���A
  // �X�e�B�b�N�̐������͒l��O�����ɔ��f���A
  // �i�s�x�N�g�����v�Z����
  inputVector.x = forwardXZ.x * ay + rightXZ.x * ax;
  inputVector.y = 0;
  inputVector.z = forwardXZ.y * ay + rightXZ.y * ax;
}

void CompPlayer::UpdateUiParameter()
{
  // �X���X�^�[�e��
  uiParams.thrusterQuantity = thrusterManager.GetThrusterQuantity() / ThrusterManager::THRUSTER_CAPACITY;

  // UI�̌��h���ׁ̈A10�{����
  uiParams.elevation = gameObject.lock()->transform.position.y + ELEVATION_OFFSET * 10.0f;
  Mathf::Normalize(velocity, uiParams.moveSpeed);
  uiParams.moveSpeed *= 10.0f;

  // ���e�B�N���̈ʒu
  if (attackTarget.expired() == false) {
    uiParams.isRockOn = isRockOn;
    uiParams.isTarget = !isRockOn;
  }
  else {
    uiParams.isRockOn = false;
    uiParams.isTarget = false;
  }

  uiParams.healthRate = health / MaxHealth;
  // �_���[�W���󂯂��ۂ̐ԃQ�[�W�̋���
  uiParams.damage = std::lerp(uiParams.damage, uiParams.healthRate, 0.1f);
}