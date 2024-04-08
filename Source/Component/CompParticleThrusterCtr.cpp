#include "CompParticleThrusterCtr.h"
#include <imgui.h>

CompParticleThrusterCtr::CompParticleThrusterCtr(GameObject* owner)
{
  ownerComp = owner->GetComponent<CompArmoredCore>();
}

void CompParticleThrusterCtr::Start()
{
  emitterComp = GetGameObject()->GetComponent<CompParticleEmitter>();

  emitVolume = emitterComp.lock()->GetEmitRate();
}

void CompParticleThrusterCtr::Update(const float& elapsedTime)
{
  // �u�[�X�g���[�h�łȂ���Ή������Ȃ�
  if (ownerComp.lock()->GetBoostMode() == false)
  {
    emitterComp.lock()->SetIsLoop(false);
    return;
  }
  emitterComp.lock()->SetIsLoop(true);

  // �ߐڍU�����̓p�[�e�B�N���𐶐�����
  if (ownerComp.lock()->GetIsSlashEffect() == true) {
    emitterComp.lock()->SetEmitRate(emitVolume);
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
    if (dot > acceptRange) {
      // �p�[�e�B�N���̐��������Ȃ�
      emitterComp.lock()->SetEmitRate(0);
    }
    else {
      emitterComp.lock()->SetEmitRate(emitVolume);
    }
  }
}

void CompParticleThrusterCtr::DrawImGui()
{
  ImGui::DragFloat("AcceptRange", &acceptRange, 0.001f, -1.0f, 1.0f);
}
