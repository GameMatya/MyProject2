#include "TimeManager.h"

void TimeManager::Update(const float& elapsedTime)
{
  effectTime -= elapsedTime;

  if (effectTime < 0.0f) {
    timeScale = 1.0f;
  }
}
