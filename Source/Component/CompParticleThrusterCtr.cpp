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
  // ブーストモードでなければ何もしない
  if (ownerComp.lock()->GetBoostMode() == false)
  {
    emitterComp.lock()->SetIsLoop(false);
    return;
  }
  emitterComp.lock()->SetIsLoop(true);

  // 近接攻撃中はパーティクルを生成する
  if (ownerComp.lock()->GetIsSlashEffect() == true) {
    emitterComp.lock()->SetEmitRate(emitVolume);
    return;
  }

  // 移動方向とエミッターの方向によってパーティクルの生成数を増減させる
  {
    ObjectTransform& transform = emitterComp.lock()->GetGameObject()->transform;

    // 移動方向
    DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&ownerComp.lock()->GetInputVec());

    // パーティクルの飛翔方向
    DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);
    Rotation = DirectX::XMVector3TransformNormal(Rotation, *transform.GetParentTransform());
    Rotation = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(Rotation, 0));

    // 内積で各ベクトルの角度差を求める
    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(MoveVec, Rotation));
    if (dot > acceptRange) {
      // パーティクルの生成をしない
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
