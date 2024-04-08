#pragma once

#include <memory>
#include "Graphics/RendererBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/Graphics.h"
#include "Graphics/post_process/PostprocessRenderer.h"

#include "Scene.h"

class SceneRenderer :public RendererBase
{
private:
  SceneRenderer();

public:
  static SceneRenderer& Instance() {
    static SceneRenderer instance;
    return instance;
  }

  void SetElapsedTime(const float& time) { elapsedTime = time; totalTime += elapsedTime; }
  float GetElapsedTime() { return elapsedTime; }

  void UpdateConstants(ID3D11DeviceContext* dc);

  void Render3D(ID3D11DeviceContext* dc, Scene* scene);

  void RenderImGui();

  // ポストプロセス
  void RenderPostprocess(ID3D11DeviceContext* dc, std::vector<PostShader*> postShaders, PostProcessContext postContext);

#ifdef _DEBUG
  RenderTarget* GetG_Color() { return gBufferColor.get(); }
  RenderTarget* GetG_Normal() { return gBufferNormal.get(); }
  RenderTarget* GetG_Pos() { return gBufferPosition.get(); }
  RenderTarget* GetG_MRAO() { return gBufferMetalRoughAmbient.get(); }

#endif // _DEBUG

private:
  // 空の描画
  void RenderSkyBox(ID3D11DeviceContext* dc, RenderContext& rc);

  // デファードレンダリング
  void RenderDeferred2Pass(ID3D11DeviceContext* dc, RenderContext& rc);

private:
  float elapsedTime = 0;
  float totalTime = 0;

  // デファードレンダリング用レンダーターゲット
  std::unique_ptr<RenderTarget> gBufferColor;
  std::unique_ptr<RenderTarget> gBufferNormal;
  std::unique_ptr<RenderTarget> gBufferPosition;
  std::unique_ptr<RenderTarget> gBufferMetalRoughAmbient;
  std::unique_ptr<RenderTarget> gBufferEmissive;
  std::unique_ptr<DepthStencil> gBufferDepth;

  // オフスクリーンバッファ
  std::shared_ptr<RenderTarget> offScreenTarget;

  //  ポストプロセス
  std::unique_ptr<PostprocessRenderer>	postprocessingRenderer;

};