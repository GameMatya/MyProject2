#pragma once

#include <vector>
#include <memory>

#include "StaticModelRenderer.h"
#include "ShadowMapRenderer.h"
#include "Component/CompModel.h"

class ModelRenderer
{
public:
  ModelRenderer() {}
  ~ModelRenderer() { Clear(); }

  // ���I���f���̕`��
  void RenderDynamic(ID3D11DeviceContext* dc, const RenderContext& rc, ModelShader* shader);
  // �ÓI���f���̕`��
  void RenderStatic(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);
  // �V���h�E�}�b�v�̏�������
  void RenderShadow() { shadowRenderer.Render(this); }

  //	 �`�惂�f���̓o�^
  void Register(CompModel* model);

  //	 �`�惂�f���̓o�^����
  void Remove(CompModel* model);

  //	 �`�惂�f���o�^�S����
  void Clear();

  // �o�^��
  int ModelCount() { return static_cast<int>(models.size()); }

  StaticModelRenderer* GetStaticRenderer() { return &staticRenderer; }
  ShadowMapRenderer* GetShadowMapRenderer() { return &shadowRenderer; }

private:
  std::vector<CompModel*> models; // �`�惊�X�g
  StaticModelRenderer staticRenderer;
  ShadowMapRenderer   shadowRenderer;

};
