#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Graphics.h"
#include "Component/CompStaticModel.h"

// Modelクラス専用の描画クラス
class StaticModelRenderer
{
private:
  using ResourcePtr = const StaticModelResource*;

public:
  StaticModelRenderer();
  ~StaticModelRenderer() { Clear(); }

  // モデルの描画
  void Render(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);

  //	 描画モデルの登録
  void Register(CompStaticModel* model);

  //	 描画モデルの登録解除
  void Remove(CompStaticModel* model);

  //	 描画モデル登録全解除
  void Clear();

  // モデルが登録されているか
  bool HasAnyModels() { return !resources.empty(); }

private:
  // 描画リスト
  std::vector<ResourcePtr> resources;
  std::map<ResourcePtr, std::vector<GameObject*>> objectMap;

  Microsoft::WRL::ComPtr<ID3D11Buffer>        instanceBuffer;

};
