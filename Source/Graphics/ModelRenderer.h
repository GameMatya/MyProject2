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

  // 動的モデルの描画
  void RenderDynamic(ID3D11DeviceContext* dc, const RenderContext& rc, ModelShader* shader);
  // 静的モデルの描画
  void RenderStatic(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);
  // シャドウマップの書き込み
  void RenderShadow() { shadowRenderer.Render(this); }

  //	 描画モデルの登録
  void Register(CompModel* model);

  //	 描画モデルの登録解除
  void Remove(CompModel* model);

  //	 描画モデル登録全解除
  void Clear();

  // 登録数
  int ModelCount() { return static_cast<int>(models.size()); }

  StaticModelRenderer* GetStaticRenderer() { return &staticRenderer; }
  ShadowMapRenderer* GetShadowMapRenderer() { return &shadowRenderer; }

private:
  std::vector<CompModel*> models; // 描画リスト
  StaticModelRenderer staticRenderer;
  ShadowMapRenderer   shadowRenderer;

};
