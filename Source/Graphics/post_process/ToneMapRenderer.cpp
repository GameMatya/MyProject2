#include "ToneMapRenderer.h"
#include "Graphics/Graphics.h"
#include "imgui.h"

ToneMapRenderer::ToneMapRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // シェーダー生成
  copyShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/PostProcessDefaultPS.cso");
  luminanceShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/LuminanceExtractPS.cso");
  shrinkLogShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/ShrinkLogPS.cso");
  mixAvgLumiShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/MixAverageLuminancePS.cso");
  toneMapShader = std::make_unique<PostShader>("Shader/PostProcessDefaultVS.cso", "Shader/ToneMapPS.cso");

  // 半分のサイズにしていく ( 縮小バッファ法 )
  luminanceTargets[0] = std::make_unique<RenderTarget>(graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2, DXGI_FORMAT_R16_FLOAT);
  for (int i = 1; i < ToneBufferCount; ++i) {
    DirectX::XMINT2 size = DirectX::XMINT2(max(luminanceTargets[i - 1]->GetWidth() / 2, 1), max(luminanceTargets[i - 1]->GetHeight() / 2, 1));
    luminanceTargets[i] = std::make_unique<RenderTarget>(size.x, size.y, DXGI_FORMAT_R32_FLOAT);
  }

  // 平均輝度
  averageLuminance = std::make_unique<RenderTarget>(
    luminanceTargets[ToneBufferCount - 1]->GetWidth(), luminanceTargets[ToneBufferCount - 1]->GetHeight(),
    DXGI_FORMAT_R16_UNORM);

  // 1ピクセルのみ
  oldAverageLuminance = std::make_unique<RenderTarget>(
    1, 1,
    DXGI_FORMAT_R16_UNORM);
}

void ToneMapRenderer::Photometry()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  // 現在設定されているバッファを退避( 違う場所に保存 )して初期化しておく
  CacheRenderTargets(dc);

  PostShader* shader;

  // 輝度抽出・ダウンサンプリング処理
  {
    // レンダーターゲット設定
    dc->OMSetRenderTargets(1, luminanceTargets[currentBufferNum]->GetRenderTargetView().GetAddressOf(), nullptr);
    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(luminanceTargets[currentBufferNum]->GetWidth());
    viewport.Height = static_cast<float>(luminanceTargets[currentBufferNum]->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dc->RSSetViewports(1, &viewport);

    ID3D11ShaderResourceView* srv;

    // バッファ番号が0なら輝度抽出
    if (currentBufferNum == 0) {
      srv = sceneSRV;
      shader = luminanceShader.get();
    }
    // 0以外ならダウンサンプリング
    else {
      srv = luminanceTargets[currentBufferNum - 1]->GetShaderResourceView().Get();
      shader = shrinkLogShader.get();
    }

    shader->Begin(dc);
    shader->Draw(dc, &srv, 1);
    shader->End(dc);
  }

  // 前のフレームも加味した平均輝度を求める
  {
    // レンダーターゲット設定
    dc->OMSetRenderTargets(1, averageLuminance->GetRenderTargetView().GetAddressOf(), nullptr);
    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(averageLuminance->GetWidth());
    viewport.Height = static_cast<float>(averageLuminance->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dc->RSSetViewports(1, &viewport);

    ID3D11ShaderResourceView* srv[] = {
      luminanceTargets[ToneBufferCount - 1]->GetShaderResourceView().Get(),
      oldAverageLuminance->GetShaderResourceView().Get()
    };

    mixAvgLumiShader->Begin(dc);
    mixAvgLumiShader->Draw(dc, srv, ARRAYSIZE(srv));
    mixAvgLumiShader->End(dc);
  }

  // 現在の平均輝度をoldAverageに保存する
  {
    // レンダーターゲット設定
    dc->OMSetRenderTargets(1, oldAverageLuminance->GetRenderTargetView().GetAddressOf(), nullptr);
    D3D11_VIEWPORT	viewport{};
    viewport.Width = static_cast<float>(oldAverageLuminance->GetWidth());
    viewport.Height = static_cast<float>(oldAverageLuminance->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    dc->RSSetViewports(1, &viewport);

    ID3D11ShaderResourceView** srv = const_cast<ID3D11ShaderResourceView**>(averageLuminance->GetShaderResourceView().GetAddressOf());

    copyShader->Begin(dc);
    copyShader->Draw(dc, srv, 1);
    copyShader->End(dc);
  }

  //	元のバッファに戻す
  RestoreRenderTargets(dc);

  // バッファ番号を更新
  currentBufferNum++;
  currentBufferNum = currentBufferNum % ToneBufferCount;

#ifdef _DEBUG
  ImGui::Begin("ToneMap");
  for (int i = 0; i < ToneBufferCount; ++i) {
    ImGui::Image(luminanceTargets[i]->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
  }
  ImGui::End();

#endif // _DEBUG
}

void ToneMapRenderer::Execute()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  ID3D11ShaderResourceView* srvs[] = {
    sceneSRV,
    averageLuminance->GetShaderResourceView().Get()
  };

  toneMapShader->Begin(dc);
  toneMapShader->Draw(dc, srvs, 2);
  toneMapShader->End(dc);
}
