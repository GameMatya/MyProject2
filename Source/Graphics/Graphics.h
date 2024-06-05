#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

#include "Shaders/SpriteShader.h"
#include "Shaders/ModelShader.h"
#include "Shaders/StaticModelShader.h"
#include "post_process/PostShader.h"
#include "Graphics/RenderTarget.h"

#include "DebugRenderer.h"
#include "LineRenderer.h"

enum class SHADER_ID_MODEL {
  FIRST_DEFERRED,

  MAX
};

enum class SHADER_ID_POST {
  DEFAULT,
  SECOND_DEFERRED,
  SKY_BOX,
  COLOR_GRADING,
  DISTANCE_FOG,

  MAX
};

enum class SHADER_ID_SPRITE {
  DEFAULT,
  BEND_SCREEN,

  MAX
};

class Graphics
{
public:
  Graphics(HWND hWnd);
  ~Graphics();

  // インスタンス取得
  static Graphics& Instance() { return *instance; }

  // デバイス取得
  ID3D11Device* GetDevice() const { return device.Get(); }

  // デバイスコンテキスト取得
  ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

  // スワップチェーン取得
  IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

  // レンダーターゲットビュー取得
  ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

  // デプスステンシルビュー取得
  ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

  // スクリーン幅取得
  UINT GetScreenWidth() const { return screenWidth; }

  // スクリーン高さ取得
  UINT GetScreenHeight() const { return screenHeight; }

  // スクリーンサイズ取得
  DirectX::XMUINT2 GetScreenSize() const { return { screenWidth,screenHeight }; }

  // ミューテックス取得
  std::mutex& GetMutex() { return graphicMutex; }

  // シェーダー取得
  ModelShader* GetShader(const SHADER_ID_MODEL& index) { return modelShaders[(int)index].get(); }
  StaticModelShader* GetShaderStatic(const SHADER_ID_MODEL& index) { return staticModelShaders[(int)index].get(); }
  PostShader* GetShader(const SHADER_ID_POST& index) { return postShaders[(int)index].get(); }
  SpriteShader* GetShader(const SHADER_ID_SPRITE& index) { return spriteShaders[(int)index].get(); }

  // レンダーターゲット取得
  RenderTarget* GetDebugRenderTarget() { return debugRenderTarget.get(); }

  // デバッグレンダラ取得
  DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

  // ラインレンダラ取得
  LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

  // ウィンドウハンドル取得
  const HWND& GetHwnd()const { return hwnd; }

private:
  static Graphics* instance;
  CONST HWND hwnd;

  // SceneView用RenderTarget
  std::unique_ptr<RenderTarget>       debugRenderTarget;
  std::unique_ptr<DebugRenderer>      debugRenderer;
  std::unique_ptr<LineRenderer>       lineRenderer;

  std::unique_ptr<SpriteShader>       spriteShaders[(int)SHADER_ID_SPRITE::MAX] = {};
  std::unique_ptr<ModelShader>        modelShaders[(int)SHADER_ID_MODEL::MAX] = {};
  std::unique_ptr<StaticModelShader>  staticModelShaders[(int)SHADER_ID_MODEL::MAX] = {};
  std::unique_ptr<PostShader>         postShaders[(int)SHADER_ID_POST::MAX] = {};

  Microsoft::WRL::ComPtr<ID3D11Device>			      device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext>		  immediateContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain>			    swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11Texture2D>			    depthStencilBuffer;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

  UINT	screenWidth;
  UINT	screenHeight;

  std::mutex graphicMutex;

};
