#include "CompParticleLandDustCtr.h"

CompParticleLandDustCtr::CompParticleLandDustCtr(GameObject* owner)
{
  ownerComp = owner->GetComponent<CompArmoredCore>();
}

void CompParticleLandDustCtr::Start()
{
  emitterComp = GetGameObject()->GetComponent<CompParticleEmitter>();
}

void CompParticleLandDustCtr::Update(const float& elapsedTime)
{
  DirectX::XMFLOAT3 velocity = ownerComp.lock()->GetVelocity();
  velocity.y = 0;

  // �ڒn��Ԃ��ړ��l������ꍇ�A�p�[�e�B�N������
  bool isEmit = (ownerComp.lock()->GetIsGround() == true && Mathf::Dot(velocity, velocity) > 0);
  emitterComp.lock()->SetIsLoop(isEmit);

  // ���s���̓p�[�e�B�N���̐����Ԋu��ݒ肷��
  emitterComp.lock()->SetEmitInterval(0.05f * (float)!ownerComp.lock()->GetBoostMode());
}

