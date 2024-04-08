#include "System/misc.h"
#include "Graphics/GaussianBlur.h"
#include "Graphics/ShaderStaties.h"
#include "Graphics/Graphics.h"

GaussianBlur::GaussianBlur(const DirectX::XMUINT2& size, const DXGI_FORMAT& format, const BLUR_DIRECTION& direction) :blurDir(direction)
{
  // ��x�����ʂ�
  if (vertexHorizontalShader == nullptr) {
    Graphics& graphics = Graphics::Instance();
    // ���_�V�F�[�_�[
    {
      // ������
      HRESULT hr = ShaderBase::createVsFromCso(graphics.GetDevice(), "Shader/GaussianBlurHorizonVS.cso",
        vertexHorizontalShader.GetAddressOf(), nullptr, nullptr, 0);

      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

      // �c����
      hr = ShaderBase::createVsFromCso(graphics.GetDevice(), "Shader/GaussianBlurVerticalVS.cso",
        vertexVerticalShader.GetAddressOf(), nullptr, nullptr, 0);

      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // �s�N�Z���V�F�[�_�[
    {
      HRESULT hr = ShaderBase::createPsFromCso(graphics.GetDevice(), "Shader/GaussianBlurPS.cso", pixelShader.GetAddressOf());

      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
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
      desc.ByteWidth = sizeof(CBFilter);
      desc.StructureByteStride = 0;

      HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, filterConstantBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

      std::unique_lock<std::mutex> lock(graphics.GetMutex());
      ID3D11DeviceContext* dc = graphics.GetDeviceContext();
      dc->VSSetConstantBuffers(10, 1, filterConstantBuffer.GetAddressOf());
      dc->PSSetConstantBuffers(10, 1, filterConstantBuffer.GetAddressOf());
    }
  }

  // �ڂ����p�̃����_�[�^�[�Q�b�g�𐶐�
  {
    textureSize = { size.x / 4,size.y / 4 };
    InitRenderTarget(textureSize, format);
  }
}

void GaussianBlur::SetSRV(ID3D11ShaderResourceView* originSRV) {
  blurSRV = originSRV;
}

void GaussianBlur::Execute(ID3D11DeviceContext* dc, const float& BlurPower) {

  // ���ݐݒ肳��Ă���o�b�t�@��ޔ����ď��������Ă���
  CacheRenderTargets(dc);

  Begin(dc, BlurPower);

  // ������
  if (blurDir != BLUR_DIRECTION::VERTICAL) {
    //	�`����ύX
    ID3D11RenderTargetView* rtv = xBlurRenderTarget->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(xBlurRenderTarget->GetWidth());
    viewport.Height = static_cast<float>(xBlurRenderTarget->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dc->RSSetViewports(1, &viewport);

    dc->VSSetShader(vertexHorizontalShader.Get(), nullptr, 0);
    Draw(dc);

    outputSRV = xBlurRenderTarget->GetShaderResourceView().Get();
  }

  // �c����
  if (blurDir != BLUR_DIRECTION::HORIZONTAL) {
    //	 �`����ύX
    ID3D11RenderTargetView* rtv = yBlurRenderTarget->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(yBlurRenderTarget->GetWidth());
    viewport.Height = static_cast<float>(yBlurRenderTarget->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dc->RSSetViewports(1, &viewport);

    //	 �`��Ώۂ�ύX
    blurSRV = xBlurRenderTarget->GetShaderResourceView().Get();

    dc->VSSetShader(vertexVerticalShader.Get(), nullptr, 0);
    Draw(dc);

    outputSRV = yBlurRenderTarget->GetShaderResourceView().Get();
  }
  End(dc);

  //	 ���̃o�b�t�@�ɖ߂�
  RestoreRenderTargets(dc);
}

// �`��J�n
void GaussianBlur::Begin(ID3D11DeviceContext* dc, const float& blurPower)
{
  dc->PSSetShader(pixelShader.Get(), nullptr, 0);
  dc->IASetInputLayout(nullptr);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  //	 �K�E�X�t�B���^�[�l�̌v�Z
  CBFilter cbFilter;
  CalcGaussianFilter(cbFilter, blurPower);
  dc->UpdateSubresource(filterConstantBuffer.Get(), 0, 0, &cbFilter, 0, 0);

  ShaderStates& staties = ShaderStates::Instance();

  const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  dc->OMSetBlendState(staties.getBlendState(BS_MODE::NONE), blend_factor, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(staties.getDepthStencilState(DS_MODE::NONE), 0);
  dc->RSSetState(staties.getRasterizerState(RS_MODE::SOLID_CULL_NONE));
}

// �`��
void GaussianBlur::Draw(ID3D11DeviceContext* dc)
{
  dc->PSSetShaderResources(0, 1, blurSRV.GetAddressOf());
  dc->Draw(4, 0);
}

// �`��I��
void GaussianBlur::End(ID3D11DeviceContext* dc)
{
  dc->VSSetShader(nullptr, nullptr, 0);
  dc->PSSetShader(nullptr, nullptr, 0);
  dc->IASetInputLayout(nullptr);

  ID3D11ShaderResourceView* srvs[] = { nullptr,nullptr,nullptr };
  dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}

void GaussianBlur::InitRenderTarget(const DirectX::XMUINT2& TextureSize, const DXGI_FORMAT& format) {
  xBlurRenderTarget = std::make_unique<RenderTarget>(TextureSize.x, TextureSize.y, format);
  yBlurRenderTarget = std::make_unique<RenderTarget>(TextureSize.x, TextureSize.y, format);
}

void GaussianBlur::CalcGaussianFilter(CBFilter& cbFilter, const float& BlurPower)
{
  cbFilter.kernelSize = MAX_KERNEL;
  cbFilter.texel.x = 1.0f / (xBlurRenderTarget->GetWidth() * 2);
  cbFilter.texel.y = 1.0f / (xBlurRenderTarget->GetHeight() * 2);

  // �d�݂̍��v���L�^����ϐ����`����
  float total = 0;

  // ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���
  // ���[�v�ϐ���x����e�N�Z������̋���
  for (int x = 0; x < MAX_KERNEL; x++)
  {
    cbFilter.weights[x] = expf(-0.5f * (float)(x * x) / BlurPower);
    total += 2.0f * cbFilter.weights[x];
  }

  // �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���
  for (int i = 0; i < MAX_KERNEL; i++)
  {
    cbFilter.weights[i] /= total;
  }
}
