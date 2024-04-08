#pragma once

#include <memory>

#include "Graphics/RenderTarget.h"
#include "Graphics/post_process/PostShader.h"
#include "BloomRenderer.h"
#include "ToneMapRenderer.h"

// ポストプロセス用のレンダラー
class PostprocessRenderer :public RendererBase
{
public:
  PostprocessRenderer();
  ~PostprocessRenderer();

  // 描画
  void	Render(ID3D11DeviceContext* deviceContext, PostProcessContext& rc,
    std::vector<PostShader*>  shaders, ID3D11ShaderResourceView* depthTexture);

  void DrawDebugGUI();

  // シーン情報の設定
  void	SetSceneData(ID3D11ShaderResourceView* srv);

private:
  // ブルーム
  std::unique_ptr<BloomRenderer> bloomRender;

  // トーンマップ
  std::unique_ptr<ToneMapRenderer> toneMap;

  //	 汎用バッファ
  std::unique_ptr<RenderTarget>	WorkTargets[2];

};
