#include "CompGun.h"
#include "StateMachine/AcArmStates.h"
#include "CompCharacter.h"
#include "imgui.h"

// �e�̔���
void CompGun::Execute(const GameObject* target)
{
  // �e�ۂ̐����ʒu
  DirectX::XMFLOAT3 bulletPos = {};
  DirectX::XMStoreFloat3(&bulletPos, weaponNode->r[3]);

  // �e�ۂ̈ړ��������Z�o
  DirectX::XMFLOAT3 bulletVec = {};
  {
    // �U���ڕW������ꍇ
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

  // �e�̐���
  GameObjectManager* manager = gameObject.lock()->GetObjectManager();
  SharedObject bullet = bulletComp->CreateBullet(manager, attackTag, bulletVec);

  // �ʒu��ݒ�
  bullet->transform.position = bulletPos;

  // �����e�������炷
  bullets--;

  executeFlag = true;
}

void CompGun::DrawImGui()
{
  ImGui::InputInt("Magazine", &bullets);
}
