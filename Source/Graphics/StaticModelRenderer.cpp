#include "StaticModelRenderer.h"
#include "System/misc.h"

StaticModelRenderer::StaticModelRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // 頂点バッファ
  D3D11_BUFFER_DESC bufferDesc{};
  bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * MAX_STATIC_BATCH;
  bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bufferDesc.MiscFlags = 0;
  bufferDesc.StructureByteStride = 0;
  HRESULT hr = graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, instanceBuffer.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void StaticModelRenderer::Render(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader)
{
  if (HasAnyModels() == false) return;

  // 描画の準備
  shader->Begin(dc, rc);

  // 頂点バッファを設定
  {
    uint32_t stride{ sizeof(DirectX::XMFLOAT4X4) };
    uint32_t offset{ 0 };

    dc->IASetVertexBuffers(1, 1, instanceBuffer.GetAddressOf(), &stride, &offset);
  }

  // モデルの描画
  for (auto& resource : resources)
  {
    std::vector<GameObject*>& objects = objectMap[resource];

    // 定数バッファの更新
    {
      D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
      HRESULT hr = dc->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

      DirectX::XMFLOAT4X4* data = reinterpret_cast<DirectX::XMFLOAT4X4*>(mapped_subresource.pData);

      assert(objects.size() <= MAX_STATIC_BATCH);
      for (int i = 0; i < objects.size(); ++i)
      {
        DirectX::XMStoreFloat4x4(&data[i], objects[i]->transform.GetWorldTransform());
      }
      dc->Unmap(instanceBuffer.Get(), 0);
    }

    // モデルを描画
    shader->Draw(dc, resource, static_cast<const int>(objects.size()));
  }

  //	シェーダーの終了処理
  shader->End(dc);
}

void StaticModelRenderer::Register(CompStaticModel* model)
{
  ResourcePtr modelResource = model->GetResource();

  // モデルのリソースが既に登録されているか調べる
  std::vector<ResourcePtr>::iterator resourceItr = std::find(resources.begin(), resources.end(), modelResource);

  // 未登録のモデルリソースだったら
  if (resourceItr == resources.end()) {
    resources.emplace_back(modelResource);
    objectMap[modelResource].emplace_back(model->GetGameObject().get());
  }
  // モデルリソースが登録済みの場合
  else
  {
#ifdef _DEBUG 
    // 同じオブジェクトがすでに登録されていないか確認
    std::vector<GameObject*>::iterator objectItr = std::find(objectMap.at(modelResource).begin(), objectMap.at(modelResource).end(), model->GetGameObject().get());
    assert(objectItr == objectMap.at(modelResource).end());

#endif // _DEBUG

    objectMap.at(modelResource).emplace_back(model->GetGameObject().get());
  }
}

void StaticModelRenderer::Remove(CompStaticModel* model)
{
  ResourcePtr modelResource = model->GetResource();

  // イテレータを取得する
  std::vector<GameObject*>::iterator it = std::find(objectMap.at(modelResource).begin(), objectMap.at(modelResource).end(), model->GetGameObject().get());
  assert(it != objectMap.at(modelResource).end());

  objectMap.at(modelResource).erase(it);
}

void StaticModelRenderer::Clear()
{
  // second要素のクリア
  for (auto& objects : objectMap) {
    objects.second.clear();
  }

  objectMap.clear();
}
