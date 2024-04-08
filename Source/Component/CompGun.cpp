#include "CompGun.h"
#include "StateMachine/AcArmStates.h"
#include "CompCharacter.h"
#include "imgui.h"

// ’e‚Ì”­Ë
void CompGun::Execute(const GameObject* target)
{
  // ’eŠÛ‚Ì¶¬ˆÊ’u
  DirectX::XMFLOAT3 bulletPos = {};
  DirectX::XMStoreFloat3(&bulletPos, weaponNode->r[3]);

  // ’eŠÛ‚ÌˆÚ“®•ûŒü‚ğZo
  DirectX::XMFLOAT3 bulletVec = {};
  {
    // UŒ‚–Ú•W‚ª‚¢‚éê‡
    if (target != nullptr) {
      CompCharacter* character = target->GetComponentConst<CompCharacter>().get();
      assert(character != nullptr);

      DirectX::XMFLOAT3 vec = (target->transform.position + DirectX::XMFLOAT3(0, character->GetWaistHeight(), 0)) - bulletPos;
      vec = Mathf::Normalize(vec);

      bulletVec = vec;
    }
    else
    {
      bulletVec = gameObject.lock()->transform.GetParentObject().lock()->transform.forward;
    }
  }

  // ’e‚Ì¶¬
  GameObjectManager* manager = gameObject.lock()->GetObjectManager();
  SharedObject bullet = bulletComp->CreateBullet(manager, attackTag, bulletVec);

  // ˆÊ’u‚ğİ’è
  bullet->transform.position = bulletPos;

  // Š’e”‚ğŒ¸‚ç‚·
  bullets--;

  executeFlag = true;
}

void CompGun::DrawImGui()
{
  ImGui::InputInt("Magazine", &bullets);
}
