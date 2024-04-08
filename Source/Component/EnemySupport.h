#pragma once

#include "Component.h"
#include "Component/CompCharacter.h"
#include "GameSystem/ScoreManager.h"

// 敵コンポーネントの基底クラス
class EnemySupport
{
public:
  void Initialize(WeakObject owner);

  // プレイヤー索敵
  bool SearchPlayer();

  // TargetPositionをランダムに設定
  void SetRandomTarget();

  // プレイヤーに向かうベクトル
  DirectX::XMFLOAT3 GetPlayerVec();

  // TargetPositionに向かう正規化ベクトルを返す
  DirectX::XMFLOAT3 GetTargetDir();
  // TargetPositionに向かうXZ平面の方向ベクトル
  DirectX::XMFLOAT3 GetTargetDirXZ();
  // TargetPositionに向かうベクトル
  DirectX::XMFLOAT3 GetTargetVec();

  void SetTerritoryPosition(const DirectX::XMFLOAT3& position) { territoryOrigin = position; }
  void SetTerritoryRange(const float& range) { territoryRange = range; }
  void SetTargetPosition(const DirectX::XMFLOAT3& position) { targetPosition = position; }
  void SetSearchRange(const float& range) { searchRange = range; }

  bool IsAlivePlayer();

  WeakObject GetPlayer() { return player; }
  float GetPlayerHeight() { return player.lock()->GetComponent<CompCharacter>()->GetWaistHeight(); }

protected:
  WeakObject owner;

  // プレイヤーオブジェクトのポインタ
  WeakObject player;

  //  縄張りの中心点
  DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
  //  縄張り範囲
  float territoryRange = 10.0f;
  // 目標地点
  DirectX::XMFLOAT3 targetPosition = { 0,0,0 };

  // 索敵範囲
  float searchRange = 5.0f;

};
