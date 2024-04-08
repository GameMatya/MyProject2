#include "ShadowCastShader.h"
#include "Graphics/ShadowMapRenderer.h"
#include "System/Misc.h"

ShadowCastShader::ShadowCastShader(ID3D11Device* device)
{
  // 頂点シェーダー
  {
    HRESULT hr = createVsFromCso(device, "Shader/ShadowCasterVS.cso", VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

// サブセット単位で描画
void ShadowCastShader::DrawSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, DirectX::XMFLOAT4 color)
{
  dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
}
