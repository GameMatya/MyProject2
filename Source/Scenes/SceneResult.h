#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"

// リザルトシーン
class SceneResult :public Scene
{
public:
  SceneResult() {}
  ~SceneResult() override {}

  // 初期化
  void InitializeGameObjects() override;
  void Initialize() override {};

  void Finalize()override;

  // 更新処理
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // 連打防止用
  const float FIXED_TIME = 1.0f;
  float fixedTimer = 0.0f;

  //オーディオ
  std::unique_ptr<AudioSource> bgm = nullptr;

};