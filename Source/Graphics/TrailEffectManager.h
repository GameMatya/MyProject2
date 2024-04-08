#pragma once

#include "TrailEffect.h"

#include <vector>
#include <memory>

// ���ׂẴg���C���G�t�F�N�g��`�悷��Ǘ��N���X
class TrailEffectManager
{
private:
  TrailEffectManager() {}

public:
  static TrailEffectManager& Instance() {
    static TrailEffectManager instance;
    return instance;
  }

  void Register(TrailEffect* trail);
  void Remove(TrailEffect* trail);

  void Update();

  void Render(ID3D11DeviceContext* dc);

  std::mutex& GetMutex() { return mutex; }

private:
  std::mutex mutex;
  std::vector<TrailEffect*> trailEffects;

};