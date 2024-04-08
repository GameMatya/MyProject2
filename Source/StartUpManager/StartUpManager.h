#pragma once

#include "Component/CompStartUpBase.h"
#include "Audio/Audio.h"

#include <array>
#include <vector>
#include <memory>

class StartUpManager
{
public:
  static constexpr float START_UP_LINE = 2.6f;
  static constexpr float START_UP_SYSTEM_ACTIVE = 1.6f;
  static constexpr float START_UP_SIGHT = 2.8f;
  static constexpr float START_UP_MAGAZINE = 3.2f;
  static constexpr float START_UP_MASSAGE = 0.5f;
  static constexpr float START_UP_SYSTEM_WAVE = 4.2f;
  static constexpr float START_UP_OTHER = 6.5f;
  static constexpr float MASSAGE_SCROLL_SPEED = 700.0f;

  enum class START_UP_LIST {
    LINE,
    MAIN_SYSTEM,
    MODE_ACTIVE,
    SIGHT,
    MAGAZINE,
    OTHER,

    MAX
  };

public:
  StartUpManager();

  void Update(const float& elapsedTime);

  void Clear();

  void SetSprite(START_UP_LIST list, std::weak_ptr<CompStartUpBase> ui) { uiList[(int)list].push_back(ui); }

  bool* GetFlag(START_UP_LIST list) { return &startUpFlags[(int)list]; };

  bool StartUpComplete() { return (executeIndex == (int)START_UP_LIST::MAX); }

private:
  // 現在のレイヤーの演出が完了したか確認
  bool CheckStartUp();

  // 待機時間の確認 ( システム起動の演出のみ待機時間あり )
  bool StartUpSystemTime(const float& elapsedTime);

  // システム起動 スプライトの破棄
  void RemoveStartUpSystems();

private:
  enum SE_LIST
  {
    VOISE,
    DISPLAY,
    NOISE,
    LOAD_SYSTEM,

    MAX
  };

private:
  int executeIndex = 0;

  // システム起動演出後の待機時間
  const float STANDBY_TIME = 1.45f;
  float standbyTimer = 0.0f;

  // システム起動のSE
  std::unique_ptr<AudioSource> seList[SE_LIST::MAX] = {};

  std::array<bool, static_cast<int>(START_UP_LIST::MAX)> startUpFlags = { true, };

  using CompStartUpContainar = std::vector<std::weak_ptr<CompStartUpBase>>;
  std::array<CompStartUpContainar, static_cast<int>(START_UP_LIST::MAX)> uiList;

};
