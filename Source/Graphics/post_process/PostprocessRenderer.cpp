#include <imgui.h>

#include "Graphics/Graphics.h"
#include "PostprocessRenderer.h"
#include "Camera/Camera.h"

PostprocessRenderer::PostprocessRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // 描画ターゲットを生成
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

  // ポストプロセスに使う情報を設定
  PostShader::UpdatePostConstant(dc, postContext);

  // 現在設定されているバッファを退避(アドレスを保持)して初期化しておく
  CacheRenderTargets(dc);

  // ブルーム処理
  {
    // 描画先を変更
    ID3D11RenderTargetView* rtv = WorkTargets[0]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // ビューポートは退避させたものを使用
    SetViewPorts(dc);

    //	 ブルーム処理を行う
    bloomRender->Render(postContext);
  }

  // トーンマッピング処理
  {
    // 描画先を変更
    ID3D11RenderTargetView* rtv = WorkTargets[1]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // ビューポートは退避させたものを使用
    SetViewPorts(dc);

    toneMap->SetSceneSRV(WorkTargets[0]->GetShaderResourceView().Get());
    toneMap->Photometry();
    toneMap->Execute();
  }

  //	 ブルーム以外のポストプロセスを実行
  for (int i = 0; i < shaders.size(); ++i)
  {
    // 描画先を変更 ( i が奇数ならWorkTargets[0] )
    ID3D11RenderTargetView* rtv = WorkTargets[i % 2]->GetRenderTargetView().Get();
    FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->OMSetRenderTargets(1, &rtv, nullptr);

    // ビューポートは退避させたものを使用
    SetViewPorts(dc);

    ID3D11ShaderResourceView* srvs[] = {
      WorkTargets[(i + 1) % 2]->GetShaderResourceView().Get(),
      depthTexture
    };

    shaders[i]->Begin(dc);
    shaders[i]->Draw(dc, srvs, 2);
    shaders[i]->End(dc);
  }

  //	元のバッファに戻す
  RestoreRenderTargets(dc);

  // ポストプロセスの結果を描画する
  {
    PostShader* shader = graphics.GetShader(SHADER_ID_POST::DEFAULT);
    shader->Begin(dc);

    ID3D11ShaderResourceView* srv = WorkTargets[(shaders.size() + 1) % 2]->GetShaderResourceView().Get();
    shader->Draw(dc, &srv, 1);

    shader->End(dc);
  }
}

//	シーン情報の設定
void	PostprocessRenderer::SetSceneData(ID3D11ShaderResourceView* srv)
{
  bloomRender->SetSceneData(srv);
}

//	デバッグ情報の表示
void	PostprocessRenderer::DrawDebugGUI()
{
  bloomRender->ImGuiRender();
}
