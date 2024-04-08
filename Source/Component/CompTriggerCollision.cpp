#include "CompTriggerCollision.h"
#include "imgui.h"

bool CompTriggerCollision::Collide(CompTriggerCollision* other)
{
  // ‘S‚Ä‚Ì“–‚½‚è”»’è‚Æ”»’è‚ðŽæ‚é
  for (int i = 0; i < colliders.size(); ++i) {
    for (int j = 0; j < other->GetColliders().size(); ++j) {
      if (colliders.at(i)->Collision(*other->GetColliders().at(j).get(), true, 0))
        return true;
    }
  }

  return false;
}

bool CompTriggerCollision::CheckAttackType(CompTriggerCollision* other)
{
  return other->GetTag() == this->attackTag;
}

void CompTriggerCollision::DrawImGui()
{
  ImGui::Text((std::string("My Tag") + std::to_string(static_cast<int>(myTag))).c_str());
  ImGui::Text((std::string("Attack Tag") + std::to_string(static_cast<int>(attackTag))).c_str());

  for (int i = 0; i < colliders.size(); ++i) {
    colliders.at(i)->DrawDebug({ 1,0,1,1 });
  }
}