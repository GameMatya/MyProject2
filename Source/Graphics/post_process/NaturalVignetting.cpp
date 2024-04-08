#include "System/Misc.h"
#include "Graphics/post_process/NaturalVignetting.h"

NaturalVignetting::NaturalVignetting(ID3D11Device* device)
{
  // 頂点シェーダー
  {
    // ファイルを開く
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\NaturalVignetting_vs.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // ファイルのサイズを求める
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // メモリ上に頂点シェーダーデータを格納する領域を用意する
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // 頂点シェーダー生成
    HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // ピクセルシェーダー
  {
    // ファイルを開く
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\NaturalVignetting_ps.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // ファイルのサイズを求める
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // メモリ上にピクセルシェーダーデータを格納する領域を用意する
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // ピクセルシェーダー生成
    HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // 定数バッファ
  {
    // シーン用バッファ
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(Constant);
    desc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&desc, 0, ConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // ブレンドステート
  {
    D3D11_BLEND_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // 深度ステンシルステート
  {
    D3D11_DEPTH_STENCIL_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.DepthEnable = true;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // ラスタライザーステート
  {
    D3D11_RASTERIZER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.FrontCounterClockwise = false;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = true;
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.AntialiasedLineEnable = false;

    HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // サンプラステート
  {
    D3D11_SAMPLER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 1;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = -FLT_MAX;
    desc.MaxLOD = FLT_MAX;
    desc.BorderColor[0] = 1.0f;
    desc.BorderColor[1] = 1.0f;
    desc.BorderColor[2] = 1.0f;
    desc.BorderColor[3] = 1.0f;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }
}

// 描画開始
void NaturalVignetting::Begin(ID3D11DeviceContext* deviceContext)
{
  deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
  deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
  deviceContext->IASetInputLayout(inputLayout.Get());

  deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
  deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  ID3D11Buffer* constantBuffers[] =
  {
    ConstantBuffer.Get(),
  };
  deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
  deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

  const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  deviceContext->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
  deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
  deviceContext->RSSetState(rasterizerState.Get());
  deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}

// 描画
void NaturalVignetting::Render(ID3D11DeviceContext* deviceContext,const RenderContext& rc,const Sprite* sprite)
{
  Begin(deviceContext);

  Constant cb;
  cb.ASPECT_RATIO = rc.vignettingData.ASPECT_RATIO;
  cb.VIGNETTING_MECHANICAL_RADIUS2 = rc.vignettingData.MECHANICAL_RADIUS2;
  cb.VIGNETTING_MECHANICAL_INV_SMOOTHNESS = rc.vignettingData.MECHANICAL_INV_SMOOTHNESS;
  cb.VIGNETTING_MECHANICAL_SCALE = rc.vignettingData.MECHANICAL_SCALE;

  cb.VIGNETTING_NATURAL_COS_FACTOR = rc.vignettingData.NATURAL_COS_FACTOR;
  cb.VIGNETTING_NATURAL_COS_POWER = rc.vignettingData.NATURAL_COS_POWER;
  cb.VIGNETTING_NATURAL_SCALE = rc.vignettingData.NATURAL_SCALE;
  cb.VIGNETTING_COLOR = rc.vignettingData.COLOR;

  deviceContext->UpdateSubresource(ConstantBuffer.Get(), 0, 0, &cb, 0, 0);

  UINT stride = sizeof(Sprite::Vertex);
  UINT offset = 0;

  deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
  deviceContext->Draw(4, 0);

  End(deviceContext);
}

// 描画終了
void NaturalVignetting::End(ID3D11DeviceContext* deviceContext)
{
  deviceContext->VSSetShader(nullptr, nullptr, 0);
  deviceContext->PSSetShader(nullptr, nullptr, 0);
  deviceContext->IASetInputLayout(nullptr);
}
