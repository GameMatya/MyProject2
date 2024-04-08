#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"

// タイトルシーン
class SceneTitle :public Scene
{
public:
  SceneTitle() {}
  ~SceneTitle() override {}

  // 初期化
  void InitializeGameObjects() override;
  void Initialize() override;

  void Finalize()override;

  // 更新処理
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  bool pushEffect = false;

  int transitionState = 0;

  // スタートボタンを押した際の演出スプライト
  std::weak_ptr<CompBooleanSprite> effectSprite;

  //オーディオ
  std::unique_ptr<AudioSource> bgm = nullptr;

};