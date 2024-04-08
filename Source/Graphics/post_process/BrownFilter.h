#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class BrownFilterShader : public SpriteShader
{
public:
  BrownFilterShader(ID3D11Device* device);
  ~BrownFilterShader() override {}

  void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
  void Draw(ID3D11DeviceContext* dc, const RenderContext& rc, const Sprite* sprite) override;
  void End(ID3D11DeviceContext* dc) override;

private:
  struct Constant
  {
    float pixel_detail; // 画面の細かさ
    float display_flat; // 画面の出っ張り具合 range( 0~1 )
    float noise_scroll; // ノイズのスクロール値
    float dummy;
  };
  Microsoft::WRL::ComPtr<ID3D11Buffer>				ConstantBuffer;

  Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

  Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

  Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

};
