#include "TrailEffectManager.h"
#include "Graphics.h"

void TrailEffectManager::Register(TrailEffect* trail)
{
#ifdef _DEBUG
  // �����I�u�W�F�N�g�����łɓo�^����Ă��Ȃ����m�F
  std::vector<TrailEffect*>::iterator it = std::find(trailEffects.begin(), trailEffects.end(), trail);
  assert(it == trailEffects.end());
#endif // _DEBUG

  trailEffects.emplace_back(trail);
}

void TrailEffectManager::Remove(TrailEffect* trail)
{
  std::vector<TrailEffect*>::iterator it = std::find(trailEffects.begin(), trailEffects.end(), trail);
  assert(it != trailEffects.end());

  trailEffects.erase(it);
}

void TrailEffectManager::Update()
{
  for (auto& effect : trailEffects) {
    effect->Update();
  }
}

void TrailEffectManager::Render(ID3D11DeviceContext* dc)
{
  Update();

  for (auto& effect : trailEffects) {
    effect->Render(dc);
  }
}
