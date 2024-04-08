#pragma once

#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Graphics/RenderTarget.h"
#include "Graphics/RendererBase.h"

const int MAX_KERNEL = 8;

enum class BLUR_DIRECTION {
  ALL_DIRECTIONS = 0,
  HORIZONTAL,
  VERTICAL
};

class GaussianBlur :public RendererBase
{
  struct CBFilter
  {
    float	weights[MAX_KERNEL] = {};

    float	kernelSize = 0;
    DirectX::XMFLOAT2	texel = {};
    float	dummy = 0;
  };

public:
  GaussianBlur(const DirectX::XMUINT2& textureSize, const DXGI_FORMAT& format, const BLUR_DIRECTION& direction);
  ~GaussianBlur() {}

  void SetSRV(ID3D11ShaderResourceView* originSRV);

  void Execute(ID3D11DeviceContext* dc, const float& BlurPower);

  ID3D11ShaderResourceView* GetBlurSRV() { return outputSRV; }

  DirectX::XMUINT2 GetTextureSize() { return textureSize; }

private:
  void Begin(ID3D11DeviceContext* dc, const float& blurPower);
  void Draw(ID3D11DeviceContext* dc);
  void End(ID3D11DeviceContext* dc);

  void InitRenderTarget(const DirectX::XMUINT2& TextureSize, const DXGI_FORMAT& format);

  // 重みテーブル計算
  void CalcGaussianFilter(CBFilter& cbFilter, const float& BlurPower);

private:
  ID3D11ShaderResourceView* outputSRV = nullptr;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blurSRV;

  // ブラーの方向
  BLUR_DIRECTION blurDir = BLUR_DIRECTION::ALL_DIRECTIONS;

  std::unique_ptr<RenderTarget> xBlurRenderTarget;		// 横ボケ画像を描画するレンダリングターゲット
  std::unique_ptr<RenderTarget> yBlurRenderTarget;		// 縦ボケ画像を描画するレンダリングターゲット

  DirectX::XMUINT2 textureSize = {};

  inline static Microsoft::WRL::ComPtr<ID3D11Buffer>				  filterConstantBuffer;
  inline static Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexHorizontalShader;
  inline static Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexVerticalShader;
  inline static Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
};
