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

#pragma region 入力情報
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
  // ロックオン対象を探す
  void SearchTargetEnemy();

  // 移動入力の更新
  void UpdateInputMove();

  void UpdateUiParameter();

public:
  // UI制御用の構造体
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
  // 敵 関連
  const float SEARCH_DISTANCE = 60.0f;      // 索敵距離
  const float SEARCH_RANGE = 0.78f;         // 索敵範囲 ( 0~1 )

};
