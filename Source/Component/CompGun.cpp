#include "CompGun.h"
#include "StateMachine/AcArmStates.h"
#include "CompCharacter.h"
#include "imgui.h"

// 弾の発射
void CompGun::Execute(const GameObject* target)
{
  // 弾丸の生成位置
  DirectX::XMFLOAT3 bulletPos = {};
  DirectX::XMStoreFloat3(&bulletPos, weaponNode->r[3]);

  // 弾丸の移動方向を算出
  DirectX::XMFLOAT3 bulletVec = {};
  {
    // 攻撃目標がいる場合
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

  // 弾の生成
  GameObjectManager* manager = gameObject.lock()->GetObjectManager();
  SharedObject bullet = bulletComp->CreateBullet(manager, attackTag, bulletVec);

  // 位置を設定
  bullet->transform.position = bulletPos;

  // 所持弾数を減らす
  bullets--;

  executeFlag = true;
}

void CompGun::DrawImGui()
{
  ImGui::InputInt("Magazine", &bullets);
}
