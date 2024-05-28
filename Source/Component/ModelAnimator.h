#pragma once
#include "Math/OriginMath.h"
#include "Graphics/ModelResource.h"

#include <array>
#include <string>

class CompModel;

class ModelAnimator
{
public:
  enum ANIM_AREA
  {
    BOTTOM,     // 下半身
    UPPER,      // 上半身

    BOTTOM_SUB, // 下半身2

    MAX
  };

public:
  ModelAnimator(CompModel* model) :model(model) {}

  // アニメーションの更新
  void UpdateAnimations(const float& elapsedTime);

  void DrawImGui();

  // 指定したアニメーションの番号を探す
  int FindAnimationId(const std::string& name);

  // アニメーション再生
  void PlayAnimation(const ANIM_AREA& bodyArea, const int& index, const bool& isLoop, const float& blendSeconds = 0.2f); // 全身
  void PlayAnimationBottomSub(const int& index); // 下半身2

  // 下半身1と下半身2のブレンド率
  void SetBottomBlendRate(const float& rate) { animeBlendRate = rate; }

  // 腰のノード番号
  void SetSplitID(const int& spineNodeId);

  // アニメーション再生速度設定
  void SetAnimationSpeed(const ANIM_AREA& bodyArea, const float& speed) { animeSpeed[bodyArea] = (std::max)(speed, 0.0f); }

  // ループフラグの設定
  void SetAnimationLoop(const ANIM_AREA& bodyArea, const bool& isLoop) { animeLoopFlag[bodyArea] = isLoop; }

  // アニメーション再生中か
  bool IsPlayAnimation(const ANIM_AREA& bodyArea) const;

  // 現在のアニメーション再生時間取得
  float GetCurrentAnimationSeconds(const ANIM_AREA& bodyArea) const { return currentAnimeSeconds[bodyArea]; }

  // 現在のアニメーション再生割合
  float GetAnimationRate(const ANIM_AREA& bodyArea) const;

  // 現在のアニメーション番号取得
  int GetCurrentAnimationId(const ANIM_AREA& area)const { return currentAnimeIndex[area]; }

  // 現在のアニメーション再生速度取得
  float GetAnimationSpeed(const ANIM_AREA& bodyArea)const { return animeSpeed[bodyArea]; }

  bool CheckEvent(const ANIM_AREA& area, const ANIMATION_EVENT& animEvent)const;

private:
  // アニメーション更新処理
  void UpdateAnimation(const float& elapsedTime, const ANIM_AREA& bodyArea);

  // ブレンドアニメーション更新処理
  void UpdateAnimationBottomSub(const float& elapsedTime);

  // アニメーション再生時間の更新
  void UpdateAnimationSecond(const float& elapsedTime, const ANIM_AREA& bodyArea);

  // ルートモーションを適用
  void ApplyRootMotion();

  // 特定のアニメーション時間でのルートノードの姿勢を求める
  DirectX::XMFLOAT3 CalculateRootNodePos(const float& animationTime);

private:
  CompModel* model = nullptr;
  int splitNodeId = -1; // 腰のノード番号
  int rootNodeId = 9;  // ルートノードの番号

  std::array<int, ANIM_AREA::MAX>       currentAnimeIndex = { -1,-1,-1 };  // 再生中のアニメーション番号
  std::array<float, ANIM_AREA::MAX - 1> currentAnimeSeconds = {};          // アニメーションの再生時間
  std::array<float, ANIM_AREA::MAX - 1> animeSpeed = { 1,1 };              // アニメーションの再生速度

  // ルートモーション用
  float oldAnimationSecond = 0.0f; // 前フレームのアニメーション時間
  bool  bakeTranslationY = false;  // Y軸の移動量を反映するかフラグ

  // ワンショットアニメーション変数
  std::array<bool, ANIM_AREA::MAX - 1>  animeLoopFlag = {};
  std::array<bool, ANIM_AREA::MAX - 1>  animeEndFlag = {};

  // ブレンド補完
  float animeBlendRate = 0.5f; // ブレンドアニメーションのブレンド率 ( 0 ~ 1 )
  std::array<float, ANIM_AREA::MAX>     animeBlendTimer = {};   // 補完率計算用タイマー
  std::array<float, ANIM_AREA::MAX - 1> animeBlendSeconds = {}; // 補完が完了する時間

};
