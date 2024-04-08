#pragma once

#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderContext/RenderContext.h"
#include "GaussianBlur.h"
#include "RendererBase.h"
#include "Shaders/ShadowCastShader.h"
#include "Shaders/StaticModelShader.h"

#include <vector>
#include <memory>

class ModelRenderer;

// ソフトシャドウ ( PCF )
class ShadowMapRenderer : public RendererBase
{
public:
  // カスケードシャドウの回数
  static const int SHADOW_MAP_COUNT = 3;

private:
  // 定数バッファ構造体
  struct ShadowCB
  {
    int index = 0;
    DirectX::XMFLOAT3 dummy;
    DirectX::XMFLOAT4X4 cascadedLVP[SHADOW_MAP_COUNT]; // ライトビュープロジェクション行列
  };

public:
  ShadowMapRenderer();

  // シャドウマップに描画
  void Render(ModelRenderer* renderer);

  void DrawImGui();

private:
  // 定数バッファ更新
  void DrawShadowMap(ID3D11DeviceContext* dc, ModelRenderer* renderer);

  // クロップ行列と乗算したライトビュープロジェクション行列を算出
  void CalcCascadedLVP(ShadowCB& cb);

private:
  const float Near = 1.0f;
  const float Far = 300.0f;
  static const int RENDER_TARGET_SIZE = 3200;

  float cascadeArea[SHADOW_MAP_COUNT + 1] = {
    Near,
    20,
    120,
    Far
  };

  std::unique_ptr<ShadowCastShader>  shadowCastShader = nullptr;
  std::unique_ptr<StaticModelShader>  shadowCastStaticShader = nullptr;
  std::array<std::unique_ptr<DepthStencil>, SHADOW_MAP_COUNT> shadowMaps;

  float lightDistance = 100.0f;
  float drawDistance = 210.0f;

  Microsoft::WRL::ComPtr<ID3D11Buffer> shadowCB;

};
