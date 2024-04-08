#include "StartUpManager.h"

StartUpManager::StartUpManager()
{
  const char* path[SE_LIST::MAX] = {
    "Data/Audio/SE/SystemActive.wav",
    "Data/Audio/SE/DisplayUI.wav",
    "Data/Audio/SE/StartUpNoise.wav",
    "Data/Audio/SE/LoadingMainSystem.wav",
  };
  const float volume[SE_LIST::MAX] = {
    0.8f,
    0.8f,
    0.4f,
    0.7f
  };

  // SE読み込み
  for (int i = 0; i < SE_LIST::MAX; ++i) {
    seList[i] = Audio::Instance().LoadAudioSource(path[i]);
    seList[i]->setVolume(volume[i]);
  }
}

void StartUpManager::Update(const float& elapsedTime)
{
  // スタートアップが完了したら何もしない
  if (executeIndex == (int)START_UP_LIST::MAX) return;

  // 各UIのスタートアップ完了を確認
  bool isNext = CheckStartUp();

  // システム起動の演出なら
  if (executeIndex == (int)START_UP_LIST::MODE_ACTIVE) {
    // 待機時間の確認
    isNext = StartUpSystemTime(elapsedTime);
  }

  // 次のUIに遷移する
  if (isNext == true) {
    // システム起動の演出が終了したら破棄
    if (executeIndex == (int)START_UP_LIST::MODE_ACTIVE) {
      RemoveStartUpSystems();
    }

    // システム起動のSE再生
    if (executeIndex == (int)START_UP_LIST::LINE) {
      seList[SE_LIST::VOISE]->Play(false);
      seList[SE_LIST::LOAD_SYSTEM]->Play(false);
      seList[SE_LIST::NOISE]->Play(false);
    }
    // UI表示のSE再生
    if (executeIndex == (int)START_UP_LIST::SIGHT) {
      seList[SE_LIST::DISPLAY]->Play(false);
    }

    // 次の実行対象へ
    executeIndex++;

    // スタートアップが完了したら何もしない
    if (executeIndex == (int)START_UP_LIST::MAX) {
      // SEを破棄
      for (int i = 0; i < SE_LIST::MAX; ++i) {
        seList[i].reset();
      }

      return;
    }

    // 次のフラグを有効化する
    startUpFlags[executeIndex] = true;
  }
}

void StartUpManager::Clear()
{
  for (int i = 0; i < (int)START_UP_LIST::MAX; ++i) {
    uiList[i].clear();
  }
}

bool StartUpManager::CheckStartUp()
{
  for (auto& ui : uiList[executeIndex]) {
    if (ui.lock()->GetStartUpRate() < 1.0f) {
      return false;
    }
  }

  return true;
}

bool StartUpManager::StartUpSystemTime(const float& elapsedTime)
{
  standbyTimer += elapsedTime;

  return (standbyTimer >= STANDBY_TIME);
}

void StartUpManager::RemoveStartUpSystems()
{
  for (int i = 0; i <= (int)START_UP_LIST::MODE_ACTIVE; ++i) {
    for (auto& ui : uiList[i]) {
      ui.lock()->GetGameObject()->Destroy();
    }
    uiList[i].clear();
    startUpFlags[i] = false;
  }
}
