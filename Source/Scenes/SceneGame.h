#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"
#include "GameSystem/EnemySpawner.h"
#include "Audio/Audio.h"

// タイトルシーン
class SceneGame :public Scene
{
public:
  SceneGame() { instance = this; }
  ~SceneGame() override {}

  static const SceneGame* Instance() { return instance; }

  // 初期化
  void InitializeGameObjects() override;
  void Initialize() override;

  // 終了化
  void Finalize() override;

  // 更新処理
  void Update(const float& elapsedTime) override;

  const bool StandbyComplete() const { return (standbyTimer > TIME_STANDBY); }

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // 待機時間の更新
  bool UpdateStandbyTime(const float& elapsedTime);

  // 制限時間の更新
  void UpdateTimeLimit(const float& elapsedTime);

private:
  inline static SceneGame* instance;

  // ゲーム開始までの待機時間
  const float TIME_STANDBY = 4.2f;
  float standbyTimer = 0.0f;

  // ゲームの制限時間＋進行時間
  const float TIME_LIMIT = 70.0f;
  float gameTimer = TIME_LIMIT;

  // 制限時間のUI
  float uiMinute = 0.0f;
  float uiSecond = 0.0f;
  float uiMilliSecond = 0.0f;

  //オーディオ
  std::unique_ptr<AudioSource> bgm = nullptr;

};