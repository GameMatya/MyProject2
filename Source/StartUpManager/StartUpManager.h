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
  // ���݂̃��C���[�̉��o�������������m�F
  bool CheckStartUp();

  // �ҋ@���Ԃ̊m�F ( �V�X�e���N���̉��o�̂ݑҋ@���Ԃ��� )
  bool StartUpSystemTime(const float& elapsedTime);

  // �V�X�e���N�� �X�v���C�g�̔j��
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

  // �V�X�e���N�����o��̑ҋ@����
  const float STANDBY_TIME = 1.45f;
  float standbyTimer = 0.0f;

  // �V�X�e���N����SE
  std::unique_ptr<AudioSource> seList[SE_LIST::MAX] = {};

  std::array<bool, static_cast<int>(START_UP_LIST::MAX)> startUpFlags = { true, };

  using CompStartUpContainar = std::vector<std::weak_ptr<CompStartUpBase>>;
  std::array<CompStartUpContainar, static_cast<int>(START_UP_LIST::MAX)> uiList;

};
