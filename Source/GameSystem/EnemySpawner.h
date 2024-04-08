#pragma once

#include "SceneSystem/Scene.h"

class EnemySpawner
{
public:
  void Initialize(Scene* scene);

  void Update(const float& elapsedTime, Scene* scene);

private:
  void SpawnHelicopter(Scene* scene);

  void SpawnAC(Scene* scene);

private:
  const DirectX::XMFLOAT3 HELICOPTER_SCALE = { 0.003f,0.003f,0.003f };
  const DirectX::XMFLOAT3 AC_SCALE = { 0.015f,0.015f,0.015f };
  const DirectX::XMFLOAT3 AC_GUN_SCALE = { 0.15f,0.15f,0.15f };

  const int HELICOPTER_VOLUME = 10;
  const DirectX::XMFLOAT3 TERRITORY_POS = { -12.0f,3.0f,12.0f };
  const float TERRITORY_RANGE = 50.0f;
  const float SPAWN_DISTANCE = 60.0f;

};
