#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Graphics.h"
#include "Component/CompStaticModel.h"

// ModelƒNƒ‰ƒXê—p‚Ì•`‰æƒNƒ‰ƒX
class StaticModelRenderer
{
private:
  using ResourcePtr = const StaticModelResource*;

public:
  StaticModelRenderer();
  ~StaticModelRenderer() { Clear(); }

  // ƒ‚ƒfƒ‹‚Ì•`‰æ
  void Render(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);

  //	 •`‰æƒ‚ƒfƒ‹‚Ì“o˜^
  void Register(CompStaticModel* model);

  //	 •`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
  void Remove(CompStaticModel* model);

  //	 •`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
  void Clear();

  // ƒ‚ƒfƒ‹‚ª“o˜^‚³‚ê‚Ä‚¢‚é‚©
  bool HasAnyModels() { return !resources.empty(); }

private:
  // •`‰æƒŠƒXƒg
  std::vector<ResourcePtr> resources;
  std::map<ResourcePtr, std::vector<GameObject*>> objectMap;

  Microsoft::WRL::ComPtr<ID3D11Buffer>        instanceBuffer;

};
