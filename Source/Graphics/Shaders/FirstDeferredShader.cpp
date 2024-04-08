#include "System/Misc.h"
#include "FirstDeferredShader.h"

FirstDeferredShader::FirstDeferredShader(ID3D11Device* device) 
{
  // 頂点シェーダー
  {
    HRESULT hr = createVsFromCso(device, "Shader/ModelDefaultVS.cso", VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // ピクセルシェーダー
  {
    HRESULT hr = createPsFromCso(device, "Shader/FirstDeferredPS.cso", PS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

}

// サブセット単位で描画
void FirstDeferredShader::DrawSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, DirectX::XMFLOAT4 color)
{
  CbSubset cbSubset;
  cbSubset.materialColor = subset.material->color * color;
  dc->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);

  ID3D11ShaderResourceView* srvs[] =
  {
    subset.material->diffuse.Get(),
    subset.material->normal.Get(),
    subset.material->metallic.Get(),
    subset.material->roughness.Get(),
    subset.material->ambientOcclusion.Get(),
    subset.material->emissive.Get(),
  };
  dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

  dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
}
