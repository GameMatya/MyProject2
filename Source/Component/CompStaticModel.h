#pragma once

#include "CompModelBase.h"
#include "Graphics/StaticModelResource.h"

class StaticModelRenderer;

// ���f��
class CompStaticModel : public CompModelBase
{
public:
  CompStaticModel(const char* filepath, ModelRenderer* modelRenderer);
  ~CompStaticModel() {}

  // ���O�擾
  const char* GetName() const override { return "StaticModelComponent"; };

  void Start()override;

  void Update(const float& elapsedTime)override {}

  void OnDestroy()override;

  // ModelResource::Collision����ɓ����蔻���ǉ�
  void AddCompCollisions()override;

  // ���\�[�X�擾
  const StaticModelResource* GetResource() const { return resource.get(); }

private:
  std::shared_ptr<StaticModelResource>	resource; // 3D���f���̑S�����i�[
  StaticModelRenderer* renderer = nullptr;

};
