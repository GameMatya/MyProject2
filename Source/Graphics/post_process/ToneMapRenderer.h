#pragma once

#include <memory>

#include "PostShader.h"
#include "Graphics/RendererBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/texture.h"

// 最後の要素は前のフレームの平均輝度を保持しておくための物
#define ToneBufferCount (10)

class ToneMapRenderer :public RendererBase
{
public:
  ToneMapRenderer();

  // 測光処理
  void Photometry();

  // トーンマッピング実行
  void Execute();

  void SetSceneSRV(ID3D11ShaderResourceView* srv) { sceneSRV = srv; }

private:
  std::unique_ptr<PostShader> copyShader;
  std::unique_ptr<PostShader> luminanceShader;
  std::unique_ptr<PostShader> shrinkLogShader;
  std::unique_ptr<PostShader> mixAvgLumiShader;
  std::unique_ptr<PostShader> toneMapShader;

  int currentBufferNum = 0;

  ID3D11ShaderResourceView* sceneSRV = nullptr;

  // 平均輝度 ( 縮小バッファ法 )
  std::unique_ptr<RenderTarget> luminanceTargets[ToneBufferCount];

  // 最終的な平均輝度
  std::unique_ptr<RenderTarget> averageLuminance;
  // 前のフレームの平均輝度
  std::unique_ptr<RenderTarget> oldAverageLuminance;

};
