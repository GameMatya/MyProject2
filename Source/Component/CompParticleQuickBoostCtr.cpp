#include "CompParticleQuickBoostCtr.h"
#include <imgui.h>

CompParticleQuickBoostCtr::CompParticleQuickBoostCtr(GameObject* owner)
{
  ownerComp = owner->GetComponent<CompArmoredCore>();
}

void CompParticleQuickBoostCtr::Start()
{
  emitterComp = GetGameObject()->GetComponent<CompParticleEmitter>();
}

void CompParticleQuickBoostCtr::Update(const float& elapsedTime)
{
  // �N�C�b�N�u�[�X�g�łȂ���Ή������Ȃ�
  if (ownerComp.lock()->GetMainSM().GetCurrentState() != CompArmoredCore::MAIN_STATE::QUICK_BOOST)
  {
    isEnterQB = false;
    return;
  }
  else if (isEnterQB == true) {
    return;
  }

  // �ړ������ƃG�~�b�^�[�̕����ɂ���ăp�[�e�B�N���̐������𑝌�������
  {
    ObjectTransform& transform = emitterComp.lock()->GetGameObject()->transform;

    // �ړ�����
    DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&ownerComp.lock()->GetInputVec());

    // �p�[�e�B�N���̔��ĕ���
    DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);
    Rotation = DirectX::XMVector3TransformNormal(Rotation, *transform.GetParentTransform());
    Rotation = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(Rotation, 0));

    // ���ςŊe�x�N�g���̊p�x�������߂�
    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(MoveVec, Rotation));
    if (dot <= acceptRange) {
      emitterComp.lock()->Play();
    }
  }
  isEnterQB = true;
}

void CompParticleQuickBoostCtr::DrawImGui()
{
  ImGui::DragFloat("AcceptRange", &acceptRange, 0.001f, -1.0f, 1.0f);
}
