#pragma once

#include "CompModelBase.h"
#include "Graphics/StaticModelResource.h"

class StaticModelRenderer;

// モデル
class CompStaticModel : public CompModelBase
{
public:
  CompStaticModel(const char* filepath, ModelRenderer* modelRenderer);
  ~CompStaticModel() {}

  // 名前取得
  const char* GetName() const override { return "StaticModelComponent"; };

  void Start()override;

  void Update(const float& elapsedTime)override {}

  void OnDestroy()override;

  // ModelResource::Collisionを基に当たり判定を追加
  void AddCompCollisions()override;

  // リソース取得
  const StaticModelResource* GetResource() const { return resource.get(); }

private:
  std::shared_ptr<StaticModelResource>	resource; // 3Dモデルの全情報を格納
  StaticModelRenderer* renderer = nullptr;

};
