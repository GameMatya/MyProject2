#include "SceneRenderer.h"
#include "Camera/Camera.h"
#include "Graphics/LightManager.h"
#include "Graphics/TrailEffectManager.h"
#include "Graphics/ModelRenderer.h"
#include "Graphics/Sprite3DRenderer.h"
#include "Graphics/ParticleManager.h"
#include "Graphics/EffectManager.h"

#include <thread>

SceneRenderer::SceneRenderer()
{
  Graphics& graphic = Graphics::Instance();
  const UINT& width = graphic.GetScreenWidth();
  const UINT& height = graphic.GetScreenHeight();

#pragma region �f�t�@�[�h�����_�����O�p�����_�[�^�[�Q�b�g
  gBufferColor = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferPosition = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
  gBufferNormal = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferMetalRoughAmbient = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferEmissive = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8_UNORM);
  gBufferDepth = std::make_unique<DepthStencil>(width, height);

#pragma endregion

#pragma region �I�t�X�N���[���o�b�t�@
  Graphics& graphics = Graphics::Instance();
  offScreenTarget = std::make_unique<RenderTarget>(graphics.GetScreenWidth(), graphics.GetScreenHeight(),
    DXGI_FORMAT_R32G32B32A32_FLOAT);

#pragma endregion

#pragma region �|�X�g�v���Z�X
  postprocessingRenderer = std::make_unique<PostprocessRenderer>();
  // �V�[���e�N�X�`����ݒ肵�Ă���
  postprocessingRenderer->SetSceneData(offScreenTarget->GetShaderResourceView().Get());

#pragma endregion

}

void SceneRenderer::UpdateConstants(ID3D11DeviceContext* dc)
{
  // �萔�o�b�t�@�X�V
  ShaderBase::UpdateSceneConstant(dc, elapsedTime, totalTime);
}

void SceneRenderer::Render3D(ID3D11DeviceContext* dc, Scene* scene)
{
  ModelRenderer& modelRenderer = scene->GetModelRenderer();

  // ���C�g�̒萔�o�b�t�@���X�V
  LightManager::Instance().UpdateConstatBuffer();

  // ���ݐݒ肳��Ă���o�b�t�@��ޔ�(�A�h���X��ێ�)���ď��������Ă���
  CacheRenderTargets(dc);

  // �V���h�E�}�b�v�ɏ�������
  modelRenderer.RenderShadow();

#pragma region ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�(G-Buffer)
  {
    ID3D11RenderTargetView* rtv[] = {
      gBufferColor.get()->GetRenderTargetView().Get(),
      gBufferNormal.get()->GetRenderTargetView().Get(),
      gBufferPosition.get()->GetRenderTargetView().Get(),
      gBufferMetalRoughAmbient.get()->GetRenderTargetView().Get(),
      gBufferEmissive.get()->GetRenderTargetView().Get()
    };
    ID3D11DepthStencilView* dsv = gBufferDepth->GetDepthStencilView().Get();

    // ��ʃN���A
    {
      FLOAT color[] = { 1.0f,0.0f,1.0f,1.0f };  // RGBA(0.0~1.0)
      for (int i = 0; i < ARRAYSIZE(rtv); ++i) {
        dc->ClearRenderTargetView(rtv[i], color);
      }
      dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
    dc->OMSetRenderTargets(ARRAYSIZE(rtv), rtv, dsv);
  }
#pragma endregion

  RenderContext rc;
  // G-Buffer�쐬
  modelRenderer.RenderStatic(dc, rc, Graphics::Instance().GetShaderStatic(SHADER_ID_MODEL::FIRST_DEFERRED));

  modelRenderer.RenderDynamic(dc, rc, Graphics::Instance().GetShader(SHADER_ID_MODEL::FIRST_DEFERRED));

#pragma region ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�(OffScreen)
  {
    ID3D11RenderTargetView* rtv = offScreenTarget->GetRenderTargetView().Get();
    ID3D11DepthStencilView* dsv = Graphics::Instance().GetDepthStencilView();

    // ��ʃN���A
    {
      FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };  // RGBA(0.0~1.0)
      dc->ClearRenderTargetView(rtv, color);
    }
    dc->OMSetRenderTargets(1, &rtv, dsv);
  }
#pragma endregion

  // �X�J�C�{�b�N�X�`��
  RenderSkyBox(dc, rc);

  // �f�t�@�[�h�����_�����O
  RenderDeferred2Pass(dc, rc);

#pragma region DepthStencilView�̍Đݒ�
  {
    ID3D11RenderTargetView* rtv = offScreenTarget->GetRenderTargetView().Get();
    ID3D11DepthStencilView* dsv = gBufferDepth->GetDepthStencilView().Get();

    dc->OMSetRenderTargets(1, &rtv, dsv);
  }
#pragma endregion

  // �g���C���G�t�F�N�g�̕`��
  TrailEffectManager::Instance().Render(dc);

  // �G�t�F�N�V�A�̕`��
  EffectManager::Instance().Render();

  // Sprite3D�̕`��
  scene->GetSprite3DRenderer().Render();

  // GPU�p�[�e�B�N���̕`��
  ParticleManager::Instance().Update();
  ParticleManager::Instance().Render();

  // �f�o�b�O���C���̕`��
#ifdef _DEBUG
  Graphics::Instance().GetLineRenderer()->Render(dc, Camera::Instance().GetView(), Camera::Instance().GetProjection());
  Graphics::Instance().GetDebugRenderer()->Render(dc, Camera::Instance().GetView(), Camera::Instance().GetProjection());
#endif // _DEBUG

  // ���̃o�b�t�@�ɖ߂�
  RestoreRenderTargets(dc);
}

void SceneRenderer::RenderSkyBox(ID3D11DeviceContext* dc, RenderContext& rc)
{
  // ���`��
  const ID3D11ShaderResourceView* skyboxTexture = LightManager::Instance().GetSkyboxTexture();
  if (skyboxTexture != nullptr)
  {
    Graphics& graphics = Graphics::Instance();
    PostShader* shader = graphics.GetShader(SHADER_ID_POST::SKY_BOX);
    shader->Begin(dc);
    // ShaderResourceView�͊���t10�ɐݒ�ς�
    shader->Draw(dc, nullptr, 0);
    shader->End(dc);
  }
}

void SceneRenderer::RenderDeferred2Pass(ID3D11DeviceContext* dc, RenderContext& rc)
{
  ID3D11ShaderResourceView* srvs[] = {
    gBufferColor->GetShaderResourceView().Get(),
    gBufferNormal->GetShaderResourceView().Get(),
    gBufferPosition->GetShaderResourceView().Get(),
    gBufferMetalRoughAmbient->GetShaderResourceView().Get(),
    gBufferEmissive->GetShaderResourceView().Get(),
    gBufferDepth->GetShaderResourceView().Get()
  };
  PostShader* shader = Graphics::Instance().GetShader(SHADER_ID_POST::SECOND_DEFERRED);
  shader->Begin(dc);
  shader->Draw(dc, srvs, ARRAYSIZE(srvs));
  shader->End(dc);
}

void SceneRenderer::RenderPostprocess(ID3D11DeviceContext* dc, std::vector<PostShader*> postShaders, PostProcessContext postContext)
{
  //	 �|�X�g�v���Z�X�������s��
  postprocessingRenderer->Render(dc, postContext, postShaders, gBufferDepth->GetShaderResourceView().Get());
}

void SceneRenderer::RenderImGui()
{
  ImGui::Begin("SceneRenderer");
  if (ImGui::TreeNode("G-Buffer"))
  {
    ImGui::Image(gBufferColor.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Image(gBufferNormal.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Image(gBufferPosition.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Image(gBufferMetalRoughAmbient.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Image(gBufferEmissive.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::Image(gBufferDepth.get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::TreePop();
  }
  ImGui::End();

  postprocessingRenderer->DrawDebugGUI();

}
