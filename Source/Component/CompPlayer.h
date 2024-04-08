#pragma once

#include "CompArmoredCore.h"
#include "Graphics/TrailEffect.h"

static const float ELEVATION_OFFSET = 10.0f;

class CompPlayer :public CompArmoredCore
{
public:
  CompPlayer() {}

  const char* GetName() const { return "Player"; }

  void Start()override;

  void Update(const float& elapsedTime)override;

  void DrawImGui()override;

#pragma region ���͏��
  void InputRockOn();
  void InputReload();
  bool InputBoostMode()  override;
  bool InputQuickBoost() override;
  bool InputJump()       override;
  bool InputRise(const float& elapsedTime) override;
  bool InputRightHand()  override;
  bool InputLeftHand()   override;

#pragma endregion

private:
  // ���b�N�I���Ώۂ�T��
  void SearchTargetEnemy();

  // �ړ����͂̍X�V
  void UpdateInputMove();

  void UpdateUiParameter();

public:
  // UI����p�̍\����
  struct UiParameter {
    float moveSpeed = 0.0f;
    float elevation = 0.0f;
    float thrusterQuantity = 0.0f;
    bool  isTarget = false;
    bool  isRockOn = false;
    float healthRate = 0.0f;
    float damage = 0.0f;

    DirectX::XMFLOAT2 screenPos = {};
  };
  UiParameter uiParams;

private:
  // �G �֘A
  const float SEARCH_DISTANCE = 60.0f;      // ���G����
  const float SEARCH_RANGE = 0.78f;         // ���G�͈� ( 0~1 )

};
