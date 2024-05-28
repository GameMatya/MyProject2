#pragma once

#include "Component.h"
#include "CompModel.h"
#include "Graphics/Effect.h"
#include "Math/OriginMath.h"

// 地球の重力
#define GRAVITY (0.98f)
// 最大落下速度
#define MAX_FALL_SPEED (-30.0f)

class CompCharacter :public Component
{
public:
  CompCharacter() {};

  virtual void Start()override;

  virtual void Update(const float& elapsedTime) override;

  // GUI描画
  virtual void DrawImGui() override;

  void OnDestroy()override;

  // 名前取得
  virtual const char* GetName() const override { return "Character"; }

  // ダメージを与える
  bool ApplyDamage(float damage, float invincibleTime);

  // 移動処理
  void Move(DirectX::XMFLOAT3 direction, float maxSpeed);

  // 旋回処理
  void Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed);

  // 向きベクトルと移動ベクトルからアニメーションを変化させる
  // ※ Enumの順番は前,後,右,左とする
  void AnimationFromVectors(DirectX::XMFLOAT3 moveVec, int baseAnime, bool isLoop, const float& blendSecond = 0.2f);

  // 衝撃を与える
  void AddImpulse(const DirectX::XMFLOAT3& impulse);

#pragma region ゲッター・セッター
  CompModel*    GetModel() { return model; }
  const float&  GetMoveSpeed()  const { return moveSpeed; }
  const float&  GetMaxMoveSpeed()  const { return maxMoveSpeed; }
  const float&  GetJumpSpeed()  const { return jumpSpeed; }
  const float&  GetTurnSpeed()  const { return turnSpeed; }
  const bool&   GetIsGround()   const { return isGround; }
  const DirectX::XMFLOAT3& GetMoveVecXZ() const { return moveVecXZ; }
  const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }
  const float& GetWaistHeight() const { return waistHeight; }
  const float& GetHealth() const { return health; }

  void SetVelocity(const DirectX::XMFLOAT3& vec) { velocity = vec; }
  void SetAccelerate(const float& accele) { accelerate = accele; }
  void SetGravityAffect(const float& affect) { gravityAffect = affect; }

#pragma endregion

protected:
  // 速力処理更新
  void UpdateVelocity(const float& elapsedTime);

  // 着地した時に呼ばれる
  virtual void OnLanding() {}

  // ダメージを受けた時に呼ばれる
  virtual void OnDamaged();

  // 無敵時間更新
  void UpdateInvincibleTimer(const float& elapsedTime);

  // 死亡した時に呼ばれる
  virtual void OnDead() {}

private:
  // ModelResourceのキャラクターデータを読み込み、設定していく
  void SettingCharacterDatas();

  // 垂直速力更新処理
  void UpdateVerticalVelocity(const float& elapsedFrame);
  // 垂直移動更新処理
  void UpdateVerticalMove(const float& elapsedTime);

  // 水平速力更新処理
  void UpdateHorizontalVelocity(const float& elapsedFrame);
  // 水平移動更新処理
  void UpdateHorizontalMove(const float& elapsedTime);

protected:
  CompModel* model = nullptr;

  // 体力
  float MaxHealth = 1;
  float health = MaxHealth;
  // 無敵時間
  float invincibleTimer = 0.3f;

  // 重力の影響度
  float gravityAffect = 1.0f;
  // 摩擦 ( 減速用 )
  float friction = 0.5f;
  // 空中制御
  float airControl = 0.3f;

  // 傾斜率
  float slopeRate = 1.0f;
  // 腰までの高さ
  float waistHeight = 0.0f;

  // 接地しているか
  bool isGround = false;

#pragma region 移動系パラメータ
  // 歩行速度
  float moveSpeed = 5.0f;
  // 加速力
  float accelerate = 1;
  // 最大移動速度
  float maxMoveSpeed = 5;
  // 旋回速度
  float turnSpeed = DirectX::XMConvertToRadians(300);
  // ジャンプの速度
  float jumpSpeed = 1.0f;
  // 着地ステートへ遷移する落下速度
  float fallSpeed = -1.0f;

  // 移動方向ベクトル (XZ平面のみ)
  DirectX::XMFLOAT3 moveVecXZ = {};
  // 移動値
  DirectX::XMFLOAT3 velocity = {};

#pragma endregion

};
