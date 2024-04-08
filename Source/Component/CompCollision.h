#pragma once

#include <vector>
#include "Component.h"
#include "Math/Collider3D.h"

class CompCollision :public Component
{
public:
  CompCollision() {}
  ~CompCollision() {}

  // –¼‘OŽæ“¾
  const char* GetName() const override { return "Collision"; }

  bool Collide(CompCollision* other);

  // GUI•`‰æ
  void DrawImGui()override;

  void AddCollider(std::shared_ptr<Collider3D> collider) { colliders.emplace_back(collider); }
  std::vector<std::shared_ptr<Collider3D>> GetColliders() { return colliders; }

private:
  std::vector<std::shared_ptr<Collider3D>> colliders;

};
