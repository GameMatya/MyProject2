#include <imgui.h>

#include "BloomRenderer.h"
#include "Graphics/Graphics.h"

BloomRenderer::BloomRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // �V�F�[�_�[����
  luminanceShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/BloomLuminancePS.cso");
  finalPassShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/BloomFinalpassPS.cso");

  UINT width = static_cast<UINT>(graphics.GetScreenWidth()) / 2;
  UINT height = static_cast<UINT>(graphics.GetScreenHeight()) / 2;

  {
    // ���P�x���o�p�`��^�[�Q�b�g�𐶐�
    luminanceExtractRenderTarget = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
  }

  // �ڂ����N���X�̐���
  {
    //	 ���� (�ڂ����̕��Ńo�b�t�@�𔼕��ɂ��鏈��������̂ŃT�C�Y��2�{���Ă���)
    gaussianBlur[0] = std::make_unique<GaussianBlur>(DirectX::XMUINT2(width * 2, height * 2), DXGI_FORMAT_R32G32B32A32_FLOAT, BLUR_DIRECTION::ALL_DIRECTIONS);

    // 2�ڈȍ~
    for (int i = 1; i < ARRAYSIZE(gaussianBlur); ++i) {
      gaussianBlur[i] = std::make_unique<GaussianBlur>(gaussianBlur[i - 1]->GetTextureSize(), DXGI_FORMAT_R32G32B32A32_FLOAT, BLUR_DIRECTION::ALL_DIRECTIONS);
    }
  }
}

BloomRenderer::~BloomRenderer()
{

}

void	BloomRenderer::Render(PostProcessContext& ppc)
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* deviceContext = graphics.GetDeviceContext();

  // ���ݐݒ肳��Ă���o�b�t�@��ޔ����ď�����
  CacheRenderTargets(deviceContext);

  // ���P�x���o�p�o�b�t�@�ɕ`����ύX���č��P�x���o
  {
    // �`����ύX
    ID3D11RenderTargetView* rtv = luminanceExtractRenderTarget->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->ClearRenderTargetView(rtv, color);
    deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(luminanceExtractRenderTarget->GetWidth());
    viewport.Height = static_cast<float>(luminanceExtractRenderTarget->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);

    //	 ���P�x���o����
    luminanceShader->Begin(deviceContext);
    luminanceShader->Draw(deviceContext, sceneSRV.GetAddressOf(), 1);
    luminanceShader->End(deviceContext);
  }

  // ���P�x�`�挋�ʂ��ڂ��� ( �쐣���u���[�� )
  {
    // �ڂ��������̏����ݒ�
    {
      //	 ���� ( �P�x���o�������摜���g�p )
      gaussianBlur[0]->SetSRV(luminanceExtractRenderTarget->GetShaderResourceView().Get());

      // 2��ڈȍ~ ( �ڂ������摜���g�p )
      for (int i = 1; i < BLUR_PASS_COUNT; ++i) {
        gaussianBlur[i]->SetSRV(gaussianBlur[i - 1]->GetBlurSRV());
      }
    }

    // �ڂ��������̎��s
    for (int i = 0; i < BLUR_PASS_COUNT; ++i) {
      gaussianBlur[i]->Execute(deviceContext, ppc.gaussianFilterData.deviation);
    }
  }

  //	 ���̃o�b�t�@�ɖ߂�
  RestoreRenderTargets(deviceContext);

  // �ŏI�p�X
  {
    ID3D11ShaderResourceView* srvs[BLUR_PASS_COUNT + 1];
    for (int i = 0; i < BLUR_PASS_COUNT; ++i) {
      srvs[i] = gaussianBlur[i]->GetBlurSRV();
    }
    srvs[BLUR_PASS_COUNT] = sceneSRV.Get();

    //	 �ڂ��������ʂ�`�悷��
    finalPassShader->Begin(deviceContext);
    finalPassShader->Draw(deviceContext, srvs, BLUR_PASS_COUNT + 1);
    finalPassShader->End(deviceContext);
  }
}

void BloomRenderer::ImGuiRender()
{
  ImGui::Begin("SceneRenderer");
  if (ImGui::TreeNode("BloomBlur")) {
    for (int i = 0; i < BLUR_PASS_COUNT; ++i) {
      ImGui::Image(gaussianBlur[i].get()->GetBlurSRV(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    }
    ImGui::TreePop();
  }
  ImGui::End();
}

//	�V�[�����̐ݒ�
void	BloomRenderer::SetSceneData(ID3D11ShaderResourceView* srv)
{
  sceneSRV = srv;
}
