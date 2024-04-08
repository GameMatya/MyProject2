#pragma once

#include <memory>

#include "Graphics/RendererBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderContext/RenderContext.h"
#include "Graphics/GaussianBlur.h"
#include "Graphics/post_process/PostShader.h"

#define BLUR_PASS_COUNT (4)

// ポストプロセス用のレンダラー
class BloomRenderer :public RendererBase
{
public:
  BloomRenderer();
  ~BloomRenderer();

  // 描画
  void	Render(PostProcessContext& ppc);

  void	ImGuiRender();

  //	シーン情報の設定
  void	SetSceneData(ID3D11ShaderResourceView* sceneSRV);

private:
  // シェーダー
  std::unique_ptr<PostShader> luminanceShader = nullptr;
  std::unique_ptr<PostShader> finalPassShader = nullptr;

  // シーン描画データ
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	sceneSRV;

  //	 高輝度抽出ぼかしバッファ
  std::unique_ptr<GaussianBlur> gaussianBlur[BLUR_PASS_COUNT];

  //	 高輝度抽出バッファ
  std::unique_ptr<RenderTarget>	luminanceExtractRenderTarget;

};
