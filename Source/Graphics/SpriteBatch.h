#pragma once

#include <wrl.h>
#include <d3d11.h>
#include "Math/OriginMath.h"
#include "Graphics/RenderContext/RenderContext.h"
#include "ShaderStaties.h"

// SpriteShaderで
class SpriteBatch
{
public:
  //--------------------------------------------------------------
  //  構造体定義
  //--------------------------------------------------------------
  struct vertex
  {
    DirectX::XMFLOAT3 position = {};
    DirectX::XMFLOAT2 texcoord = {};
  };

  // ブレンドステートの設定 (Begin の前に呼んでおく)
  void setBlendState(BS_MODE index, ID3D11DeviceContext* dc) {
    blendState = ShaderStates::Instance().getBlendState(index);
    const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
  }

  //--------------------------------------------------------------
  //  コンストラクタ
  //--------------------------------------------------------------
  SpriteBatch(size_t);
  SpriteBatch(ID3D11Device*, const char*, size_t);

  //--------------------------------------------------------------
  //  デストラクタ
  //--------------------------------------------------------------
  ~SpriteBatch() {};

  //--------------------------------------------------------------
  //  前処理
  //--------------------------------------------------------------
  void begin(ID3D11DeviceContext*);

  //--------------------------------------------------------------
  //  スプライトバッチ描画
  //--------------------------------------------------------------
  void render(
    const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
    const DirectX::XMFLOAT2& center, float angle/*radian*/,
    const DirectX::XMFLOAT4& color
  );

  //--------------------------------------------------------------
  //  後処理
  //--------------------------------------------------------------
  void end(ID3D11DeviceContext*);

private:
  bool LoadTexture(const char* filename);

private:
  const float UV_ADJUST = 1.0f;

  D3D11_VIEWPORT viewport;

  // シェーダポインタ
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = nullptr;

  Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;
  // 頂点バッファ
  Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = nullptr;

  // 画像データ
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView = nullptr;
  D3D11_TEXTURE2D_DESC        tex2dDesc = {};

  // ステート
  Microsoft::WRL::ComPtr<ID3D11BlendState> blendState = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState = nullptr;

  // 描画する最大数
  UINT MAX_INSTANCES;

  // 現在の描画数
  UINT instanceCount = 0;

  Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

  //--------------------------------------------------------------
  //  構造体定義
  //--------------------------------------------------------------
  struct instance
  {
    DirectX::XMFLOAT4X4 ndcTransform = {};
    DirectX::XMFLOAT4 texcoordTransform = {};
    DirectX::XMFLOAT4 color = {};
  };
  instance* instances = nullptr;

};
