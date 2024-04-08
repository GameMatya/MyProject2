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
  // クイックブーストでなければ何もしない
  if (ownerComp.lock()->GetMainSM().GetCurrentState() != CompArmoredCore::MAIN_STATE::QUICK_BOOST)
  {
    isEnterQB = false;
    return;
  }
  else if (isEnterQB == true) {
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
