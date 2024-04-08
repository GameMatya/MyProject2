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

  // “®“Iƒ‚ƒfƒ‹‚Ì•`‰æ
  void RenderDynamic(ID3D11DeviceContext* dc, const RenderContext& rc, ModelShader* shader);
  // Ã“Iƒ‚ƒfƒ‹‚Ì•`‰æ
  void RenderStatic(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);
  // ƒVƒƒƒhƒEƒ}ƒbƒv‚Ì‘‚«‚İ
  void RenderShadow() { shadowRenderer.Render(this); }

  //	 •`‰æƒ‚ƒfƒ‹‚Ì“o˜^
  void Register(CompModel* model);

  //	 •`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
  void Remove(CompModel* model);

  //	 •`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
  void Clear();

  // “o˜^”
  int ModelCount() { return static_cast<int>(models.size()); }

  StaticModelRenderer* GetStaticRenderer() { return &staticRenderer; }
  ShadowMapRenderer* GetShadowMapRenderer() { return &shadowRenderer; }

private:
  std::vector<CompModel*> models; // •`‰æƒŠƒXƒg
  StaticModelRenderer staticRenderer;
  ShadowMapRenderer   shadowRenderer;

};
