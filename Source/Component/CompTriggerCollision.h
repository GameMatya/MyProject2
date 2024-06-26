#pragma once

#include <vector>
#include "Component.h"
#include "Math/Collider3D.h"

enum class COLLIDER_TAGS {
  PLAYER,
  ENEMY,
  BULLET,
  PLAYER_ATTACK,
  ENEMY_ATTACK,
};

class CompTriggerCollision :public Component
{
public:
  CompTriggerCollision(COLLIDER_TAGS self, COLLIDER_TAGS attack) :myTag(self), attackTag(attack) {}

  // 名前取得
  const char* GetName() const override { return "TriggerCollision"; }

  // 衝突判定
  bool Collide(CompTriggerCollision* other);

  // 相手が衝突対象か確認
  bool CheckAttackType(CompTriggerCollision* other);

  // GUI描画
  void DrawImGui()override;

  void AddCollider(std::shared_ptr<Collider3D> collider) { colliders.emplace_back(collider); }
  std::vector<std::shared_ptr<Collider3D>> GetColliders() { return colliders; }

  COLLIDER_TAGS GetTag() { return myTag; }
  COLLIDER_TAGS GetAttackTag() { return attackTag; }

  void SetTag(const COLLIDER_TAGS& tag) { myTag = tag; }
  void SetAttackTag(const COLLIDER_TAGS& tag) { attackTag = tag; }

private:
  COLLIDER_TAGS myTag;
  COLLIDER_TAGS attackTag;

  std::vector<std::shared_ptr<Collider3D>> colliders;

};
