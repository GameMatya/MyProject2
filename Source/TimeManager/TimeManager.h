#pragma once

class TimeManager
{
private:
  TimeManager() {}

public:
  static TimeManager& Instance() {
    static TimeManager instance;
    return instance;
  }

  void Update(const float& elapsedTime);

  void SetTimeEffect(const float& scale, const float& time) {
    timeScale = scale;
    effectTime = time;
  }

  float GetElapsedTime(const float& elapsedTime) {
    return elapsedTime * timeScale;
  }

private:
  float timeScale = 1.0f;
  float effectTime = 0.0f;

};
