#include "PostShader.h"
#include "Graphics/Graphics.h"
#include "System/misc.h"

PostShader::PostShader() :ShaderBase()
{
  if (postConstantBuffer == nullptr) {
    Graphics& graphics = Graphics::Instance();
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    HRESULT hr;

    // メッシュ用バッファ
    desc.ByteWidth = sizeof(CbPostProcess);
    hr = graphics.GetDevice()->CreateBuffer(&desc, 0, postConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    std::unique_lock<std::mutex> lock(graphics.GetMutex());

    // 定数バッファ設定
    graphics.GetDeviceContext()->VSSetConstantBuffers(9, 1, postConstantBuffer.GetAddressOf());
    graphics.GetDeviceContext()->PSSetConstantBuffers(9, 1, postConstantBuffer.GetAddressOf());
  }
}

PostShader::PostShader(const char* vsFilepath, const char* psFilepath) :PostShader()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // 頂点シェーダー
  {
    HRESULT hr = createVsFromCso(device, vsFilepath, VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // ピクセルシェーダー
  {
    HRESULT hr = createPsFromCso(device, psFilepath, PS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void PostShader::UpdatePostConstant(ID3D11DeviceContext* dc, const PostProcessContext& ppc)
{
  // 定数バッファ更新
  CbPostProcess cbPost;
  cbPost.luminance.intensity = ppc.bloomData.intensity;
  cbPost.luminance.threshold = ppc.bloomData.threshold;
  cbPost.luminance.maxLuminance = ppc.bloomData.maxLuminance;

  cbPost.colorGrading.hueShift = ppc.colorGradingData.hueShift;
  cbPost.colorGrading.saturation = ppc.colorGradingData.saturation;
  cbPost.colorGrading.brightness = ppc.colorGradingData.brightness;

  cbPost.distanceFog.color = ppc.distanceFogData.color;

  dc->UpdateSubresource(postConstantBuffer.Get(), 0, 0, &cbPost, 0, 0);
}

void PostShader::Begin(ID3D11DeviceContext* dc)
{
  Activate(dc);
  dc->IASetInputLayout(nullptr);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  SetUpStates(dc, BS_MODE::NONE, DS_MODE::NONE, RS_MODE::SOLID_CULL_NONE);
}

void PostShader::Draw(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** sceneSrv, const int ArraySize)
{
  dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
  dc->PSSetShaderResources(0, ArraySize, sceneSrv);
  dc->Draw(4, 0);
}

void PostShader::End(ID3D11DeviceContext* dc)
{
  Inactivate(dc);
}
