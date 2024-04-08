#include <imgui.h>

#include "BloomRenderer.h"
#include "Graphics/Graphics.h"

BloomRenderer::BloomRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // シェーダー生成
  luminanceShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/BloomLuminancePS.cso");
  finalPassShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/BloomFinalpassPS.cso");

  UINT width = static_cast<UINT>(graphics.GetScreenWidth()) / 2;
  UINT height = static_cast<UINT>(graphics.GetScreenHeight()) / 2;

  {
    // 高輝度抽出用描画ターゲットを生成
    luminanceExtractRenderTarget = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
  }

  // ぼかしクラスの生成
  {
    //	 初回 (ぼかしの方でバッファを半分にする処理があるのでサイズを2倍しておく)
    gaussianBlur[0] = std::make_unique<GaussianBlur>(DirectX::XMUINT2(width * 2, height * 2), DXGI_FORMAT_R32G32B32A32_FLOAT, BLUR_DIRECTION::ALL_DIRECTIONS);

    // 2個目以降
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

  // 現在設定されているバッファを退避して初期化
  CacheRenderTargets(deviceContext);

  // 高輝度抽出用バッファに描画先を変更して高輝度抽出
  {
    // 描画先を変更
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

    //	 高輝度抽出処理
    luminanceShader->Begin(deviceContext);
    luminanceShader->Draw(deviceContext, sceneSRV.GetAddressOf(), 1);
    luminanceShader->End(deviceContext);
  }

  // 高輝度描画結果をぼかす ( 川瀬式ブルーム )
  {
    // ぼかし処理の初期設定
    {
      //	 初回 ( 輝度抽出をした画像を使用 )
      gaussianBlur[0]->SetSRV(luminanceExtractRenderTarget->GetShaderResourceView().Get());

      // 2回目以降 ( ぼかした画像を使用 )
      for (int i = 1; i < BLUR_PASS_COUNT; ++i) {
        gaussianBlur[i]->SetSRV(gaussianBlur[i - 1]->GetBlurSRV());
      }
    }

    // ぼかし処理の実行
    for (int i = 0; i < BLUR_PASS_COUNT; ++i) {
      gaussianBlur[i]->Execute(deviceContext, ppc.gaussianFilterData.deviation);
    }
  }

  //	 元のバッファに戻す
  RestoreRenderTargets(deviceContext);

  // 最終パス
  {
    ID3D11ShaderResourceView* srvs[BLUR_PASS_COUNT + 1];
    for (int i = 0; i < BLUR_PASS_COUNT; ++i) {
      srvs[i] = gaussianBlur[i]->GetBlurSRV();
    }
    srvs[BLUR_PASS_COUNT] = sceneSRV.Get();

    //	 ぼかした結果を描画する
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

//	シーン情報の設定
void	BloomRenderer::SetSceneData(ID3D11ShaderResourceView* srv)
{
  sceneSRV = srv;
}
