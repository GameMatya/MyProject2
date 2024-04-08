#pragma once

#include "GameObject.h"
#include "Component/CompStage.h"
#include <vector>

class StageManager
{
public:
  ~StageManager() { Clear(); }

  void Initialize() {
    for (auto& obj : stageObjects) {
      stages.emplace_back(obj->GetComponent<CompStage>().get());
    }
  }

  void Register(GameObject* stage) {
    stageObjects.emplace_back(stage);
  }

  void Clear() {
    stageObjects.clear();
    stages.clear();
  }

  bool Collision(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hitResult = nullptr);

private:
  std::vector<GameObject*> stageObjects;
  std::vector<CompStage*> stages;

};
