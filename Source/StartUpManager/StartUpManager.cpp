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

  // SE�ǂݍ���
  for (int i = 0; i < SE_LIST::MAX; ++i) {
    seList[i] = Audio::Instance().LoadAudioSource(path[i]);
    seList[i]->setVolume(volume[i]);
  }
}

void StartUpManager::Update(const float& elapsedTime)
{
  // �X�^�[�g�A�b�v�����������牽�����Ȃ�
  if (executeIndex == (int)START_UP_LIST::MAX) return;

  // �eUI�̃X�^�[�g�A�b�v�������m�F
  bool isNext = CheckStartUp();

  // �V�X�e���N���̉��o�Ȃ�
  if (executeIndex == (int)START_UP_LIST::MODE_ACTIVE) {
    // �ҋ@���Ԃ̊m�F
    isNext = StartUpSystemTime(elapsedTime);
  }

  // ����UI�ɑJ�ڂ���
  if (isNext == true) {
    // �V�X�e���N���̉��o���I��������j��
    if (executeIndex == (int)START_UP_LIST::MODE_ACTIVE) {
      RemoveStartUpSystems();
    }

    // �V�X�e���N����SE�Đ�
    if (executeIndex == (int)START_UP_LIST::LINE) {
      seList[SE_LIST::VOISE]->Play(false);
      seList[SE_LIST::LOAD_SYSTEM]->Play(false);
      seList[SE_LIST::NOISE]->Play(false);
    }
    // UI�\����SE�Đ�
    if (executeIndex == (int)START_UP_LIST::SIGHT) {
      seList[SE_LIST::DISPLAY]->Play(false);
    }

    // ���̎��s�Ώۂ�
    executeIndex++;

    // �X�^�[�g�A�b�v�����������牽�����Ȃ�
    if (executeIndex == (int)START_UP_LIST::MAX) {
      // SE��j��
      for (int i = 0; i < SE_LIST::MAX; ++i) {
        seList[i].reset();
      }

      return;
    }

    // ���̃t���O��L��������
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
