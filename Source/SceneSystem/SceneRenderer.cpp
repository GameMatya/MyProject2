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

#pragma region デファードレンダリング用レンダーターゲット
  gBufferColor = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferPosition = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
  gBufferNormal = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferMetalRoughAmbient = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
  gBufferEmissive = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8_UNORM);
  gBufferDepth = std::make_unique<DepthStencil>(width, height);

#pragma endregion

#pragma region オフスクリーンバッファ
  Graphics& graphics = Graphics::Instance();
  offScreenTarget = std::make_unique<RenderTarget>(graphics.GetScreenWidth(), graphics.GetScreenHeight(),
    DXGI_FORMAT_R32G32B32A32_FLOAT);

#pragma endregion

#pragma region ポストプロセス
  postprocessingRenderer = std::make_unique<PostprocessRenderer>();
  // シーンテクスチャを設定しておく
  postprocessingRenderer->SetSceneData(offScreenTarget->GetShaderResourceView().Get());

#pragma endregion

}

void SceneRenderer::UpdateConstants(ID3D11DeviceContext* dc)
{
  // 定数バッファ更新
  ShaderBase::UpdateSceneConstant(dc, elapsedTime, totalTime);
}

void SceneRenderer::Render3D(ID3D11DeviceContext* dc, Scene* scene)
{
  ModelRenderer& modelRenderer = scene->GetModelRenderer();

  // ライトの定数バッファを更新
  LightManager::Instance().UpdateConstatBuffer();

  // 現在設定されているバッファを退避(アドレスを保持)して初期化しておく
  CacheRenderTargets(dc);

  // シャドウマップに書き込み
  modelRenderer.RenderShadow();

#pragma region 画面クリア＆レンダーターゲット設定(G-Buffer)
  {
    ID3D11RenderTargetView* rtv[] = {
      gBufferColor.get()->GetRenderTargetView().Get(),
      gBufferNormal.get()->GetRenderTargetView().Get(),
      gBufferPosition.get()->GetRenderTargetView().Get(),
      gBufferMetalRoughAmbient.get()->GetRenderTargetView().Get(),
      gBufferEmissive.get()->GetRenderTargetView().Get()
    };
    ID3D11DepthStencilView* dsv = gBufferDepth->GetDepthStencilView().Get();

    // 画面クリア
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
  // G-Buffer作成
  modelRenderer.RenderStatic(dc, rc, Graphics::Instance().GetShaderStatic(SHADER_ID_MODEL::FIRST_DEFERRED));

  modelRenderer.RenderDynamic(dc, rc, Graphics::Instance().GetShader(SHADER_ID_MODEL::FIRST_DEFERRED));

#pragma region 画面クリア＆レンダーターゲット設定(OffScreen)
  {
    ID3D11RenderTargetView* rtv = offScreenTarget->GetRenderTargetView().Get();
    ID3D11DepthStencilView* dsv = Graphics::Instance().GetDepthStencilView();

    // 画面クリア
    {
      FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };  // RGBA(0.0~1.0)
      dc->ClearRenderTargetView(rtv, color);
    }
    dc->OMSetRenderTargets(1, &rtv, dsv);
  }
#pragma endregion

  // スカイボックス描画
  RenderSkyBox(dc, rc);

  // デファードレンダリング
  RenderDeferred2Pass(dc, rc);

#pragma region DepthStencilViewの再設定
  {
    ID3D11RenderTargetView* rtv = offScreenTarget->GetRenderTargetView().Get();
    ID3D11DepthStencilView* dsv = gBufferDepth->GetDepthStencilView().Get();

    dc->OMSetRenderTargets(1, &rtv, dsv);
  }
#pragma endregion

  // トレイルエフェクトの描画
  TrailEffectManager::Instance().Render(dc);

  // エフェクシアの描画
  EffectManager::Instance().Render();

  // Sprite3Dの描画
  scene->GetSprite3DRenderer().Render();

  // GPUパーティクルの描画
  ParticleManager::Instance().Update();
  ParticleManager::Instance().Render();

  // デバッグラインの描画
#ifdef _DEBUG
  Graphics::Instance().GetLineRenderer()->Render(dc, Camera::Instance().GetView(), Camera::Instance().GetProjection());
  Graphics::Instance().GetDebugRenderer()->Render(dc, Camera::Instance().GetView(), Camera::Instance().GetProjection());
#endif // _DEBUG

  // 元のバッファに戻す
  RestoreRenderTargets(dc);
}

void SceneRenderer::RenderSkyBox(ID3D11DeviceContext* dc, RenderContext& rc)
{
  // 空を描画
  const ID3D11ShaderResourceView* skyboxTexture = LightManager::Instance().GetSkyboxTexture();
  if (skyboxTexture != nullptr)
  {
    Graphics& graphics = Graphics::Instance();
    PostShader* shader = graphics.GetShader(SHADER_ID_POST::SKY_BOX);
    shader->Begin(dc);
    // ShaderResourceViewは既にt10に設定済み
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
  //	 ポストプロセス処理を行う
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
