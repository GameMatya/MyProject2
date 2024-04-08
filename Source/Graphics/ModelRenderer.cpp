#include "ModelRenderer.h"
#include "Graphics.h"
#include <algorithm>

//	 描画モデルの登録
void ModelRenderer::Register(CompModel* model)
{
#ifdef _DEBUG
  // 同じオブジェクトがすでに登録されていないか確認
  std::vector<CompModel*>::iterator it = std::find(models.begin(), models.end(), model);
  assert(it == models.end());
#endif // _DEBUG

  models.emplace_back(model);
}

// 描画モデルの登録解除
void ModelRenderer::Remove(CompModel* model)
{
  // イテレータを取得する
  std::vector<CompModel*>::iterator it = std::find(models.begin(), models.end(), model);
  assert(it != models.end());

  models.erase(it);
}

// 描画モデル登録全解除
void ModelRenderer::Clear()
{
  models.clear();
}

void ModelRenderer::RenderDynamic(ID3D11DeviceContext* dc, const RenderContext& rc, ModelShader* shader)
{
  // 描画の準備
  shader->Begin(dc, rc);

  // モデルを描画
  for (auto& model : models) {
    for (auto& mesh : model->GetResource()->GetMeshes()) {
      for (auto& subset : mesh.subsets) {
        //	 頂点・インデックスバッファ等設定
        shader->SetBuffers(dc, model->GetNodes(), mesh);

        //	 サブセット単位で描画
        shader->DrawSubset(dc, subset);
      }
    }
  }

  //	シェーダーの終了処理
  shader->End(dc);
}

void ModelRenderer::RenderStatic(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader)
{
  staticRenderer.Render(dc, rc, shader);
}
