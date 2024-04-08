#include <imgui.h>

#include "Graphics/Graphics.h"
#include "PostprocessRenderer.h"
#include "Camera/Camera.h"

PostprocessRenderer::PostprocessRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // �`��^�[�Q�b�g�𐶐�
  {
    UINT width = static_cast<UINT>(graphics.GetScreenWidth());
    UINT height = static_cast<UINT>(graphics.GetScreenHeight());

    WorkTargets[0] = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
    WorkTargets[1] = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  }

  bloomRender = std::make_unique<BloomRenderer>();
  toneMap = std::make_unique<ToneMapRenderer>();
}

PostprocessRenderer::~PostprocessRenderer()
{
}

void	PostprocessRenderer::Render(ID3D11DeviceContext* dc, PostProcessContext& postContext,
  std::vector<PostShader*> shaders, ID3D11ShaderResourceView* depthTexture)
{
  Graphics& graphics = Graphics::Instance();

  // �|�X�g�v���Z�X�Ɏg������ݒ�
  PostShader::UpdatePostConstant(dc, postContext);

  // ���ݐݒ肳��Ă���o�b�t�@��ޔ�(�A�h���X��ێ�)���ď��������Ă���
  CacheRenderTargets(dc);

  // �u���[������
  {
    // �`����ύX
    ID3D11RenderTargetView* rtv = WorkTargets[0]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // �r���[�|�[�g�͑ޔ����������̂��g�p
    SetViewPorts(dc);

    //	 �u���[���������s��
    bloomRender->Render(postContext);
  }

  // �g�[���}�b�s���O����
  {
    // �`����ύX
    ID3D11RenderTargetView* rtv = WorkTargets[1]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // �r���[�|�[�g�͑ޔ����������̂��g�p
    SetViewPorts(dc);

    toneMap->SetSceneSRV(WorkTargets[0]->GetShaderResourceView().Get());
    toneMap->Photometry();
    toneMap->Execute();
  }

  //	 �u���[���ȊO�̃|�X�g�v���Z�X�����s
  for (int i = 0; i < shaders.size(); ++i)
  {
    // �`����ύX ( i ����Ȃ�WorkTargets[0] )
    ID3D11RenderTargetView* rtv = WorkTargets[i % 2]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // �r���[�|�[�g�͑ޔ����������̂��g�p
    SetViewPorts(dc);

    ID3D11ShaderResourceView* srvs[] = {
      WorkTargets[(i + 1) % 2]->GetShaderResourceView().Get(),
      depthTexture
    };

    shaders[i]->Begin(dc);
    shaders[i]->Draw(dc, srvs, 2);
    shaders[i]->End(dc);
  }

  //	���̃o�b�t�@�ɖ߂�
  RestoreRenderTargets(dc);

  // �|�X�g�v���Z�X�̌��ʂ�`�悷��
  {
    PostShader* shader = graphics.GetShader(SHADER_ID_POST::DEFAULT);
    shader->Begin(dc);

    ID3D11ShaderResourceView* srv = WorkTargets[(shaders.size() + 1) % 2]->GetShaderResourceView().Get();
    shader->Draw(dc, &srv, 1);

    shader->End(dc);
  }
}

//	�V�[�����̐ݒ�
void	PostprocessRenderer::SetSceneData(ID3D11ShaderResourceView* srv)
{
  bloomRender->SetSceneData(srv);
}

//	�f�o�b�O���̕\��
void	PostprocessRenderer::DrawDebugGUI()
{
  bloomRender->ImGuiRender();
}
