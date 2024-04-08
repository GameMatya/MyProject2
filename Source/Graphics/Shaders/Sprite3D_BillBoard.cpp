#include "System/Misc.h"
#include "Sprite3D_BillBoard.h"

Sprite3D_BillBoard::Sprite3D_BillBoard(ID3D11Device* device)
{
  // ���_�V�F�[�_�[
  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\Sprite3D_BillBoard_vs.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // ���_�V�F�[�_�[����
    HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // ���̓��C�A�E�g
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // �s�N�Z���V�F�[�_�[
  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\Sprite3D_BillBoard_ps.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // �s�N�Z���V�F�[�_�[����
    HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // �u�����h�X�e�[�g
  {
    D3D11_BLEND_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    //desc.IndependentBlendEnable = false;
    //desc.AlphaToCoverageEnable = false;
    //desc.RenderTarget[0].BlendEnable = true;
    //desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    //desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    //desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    //desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    //desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    //desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    desc.IndependentBlendEnable = false;
    desc.AlphaToCoverageEnable = false;
    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // �[�x�X�e���V���X�e�[�g
  {
    D3D11_DEPTH_STENCIL_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.DepthEnable = FALSE;

    HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

  // ���X�^���C�U�[�X�e�[�g
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

  // �T���v���X�e�[�g
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

  // �萔�o�b�t�@
  {
    // �V�[���p�o�b�t�@
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(CbScene);
    desc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // �T�u�Z�b�g�p�o�b�t�@
    desc.ByteWidth = sizeof(CbMesh);

    hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
  }

}

// �`��J�n
void Sprite3D_BillBoard::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
  // Shader�ݒ�
  dc->VSSetShader(vertexShader.Get(), nullptr, 0);
  dc->PSSetShader(pixelShader.Get(), nullptr, 0);
  dc->IASetInputLayout(inputLayout.Get());

  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // Buffer�ݒ�
  ID3D11Buffer* constantBuffers[] =
  {
    sceneConstantBuffer.Get(),
    meshConstantBuffer.Get()
  };
  dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
  dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

  // �V�[���p�萔�o�b�t�@�X�V
  CbScene cbScene;
  cbScene.projection = rc.projection;
  cbScene.view = rc.view;
  dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);

  // State�ݒ�
  const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
  dc->RSSetState(rasterizerState.Get());
  dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

}

// �`��
void Sprite3D_BillBoard::Draw(ID3D11DeviceContext* dc, const Sprite3D* sprite, const DirectX::XMFLOAT4& color)
{
  CbMesh data{ sprite->GetTransform(), sprite->GetColor() };
  dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &data, 0, 0);

  ID3D11ShaderResourceView* srvs[] =
  {
    sprite->GetShaderResourceView().Get(), // �e�N�X�`���f�[�^
  };
  dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

  // ���_���
  UINT stride = sizeof(Sprite3D::vertex);
  UINT offset = 0;
  dc->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
  // �|���S�����
  dc->IASetIndexBuffer(sprite->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
  D3D11_BUFFER_DESC buffer_desc{};
  sprite->GetIndexBuffer()->GetDesc(&buffer_desc);

  dc->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);

}

// �`��I��
void Sprite3D_BillBoard::End(ID3D11DeviceContext* dc)
{
  dc->VSSetShader(nullptr, nullptr, 0);
  dc->PSSetShader(nullptr, nullptr, 0);
  dc->IASetInputLayout(nullptr);

  ID3D11ShaderResourceView* srvs[] =
  {
    nullptr,
    nullptr,
    nullptr
  };
  dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}
