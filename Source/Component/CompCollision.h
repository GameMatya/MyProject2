#pragma once

#include <vector>
#include "Component.h"
#include "Math/Collider3D.h"

class CompCollision :public Component
{
public:
  CompCollision() {}
  ~CompCollision() {}

  // ���O�擾
  const char* GetName() const override { return "Collision"; }

  bool Collide(CompCollision* other);

  // GUI�`��
  void DrawImGui()override;

  void AddCollider(std::shared_ptr<Collider3D> collider) { colliders.emplace_back(collider); }
  std::vector<std::shared_ptr<Collider3D>> GetColliders() { return colliders; }

private:
  std::vector<std::shared_ptr<Collider3D>> colliders;

};
