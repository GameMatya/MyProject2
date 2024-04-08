#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Component/CompSprite3D.h"

class Sprite3DRenderer
{
private:
  Sprite3DRenderer(const Sprite3DRenderer&)                     = delete;
  Sprite3DRenderer(Sprite3DRenderer&&)                 noexcept = delete;
  Sprite3DRenderer& operator=(Sprite3DRenderer const&)          = delete;
  Sprite3DRenderer& operator=(Sprite3DRenderer&&)      noexcept = delete;

public:
  Sprite3DRenderer();
  ~Sprite3DRenderer() { Clear(); }

  void Register(CompSprite3D* sprite);
  void Remove(CompSprite3D* sprite);
  void Clear() { sprites.clear(); }

  void Render();

private:
  const DirectX::XMINT3 BillboardAxis[static_cast<int>(BILLBOARD_AXIS::MAX)] = {
    { 0,0,0 },
    { 1,0,0 },
    { 0,1,0 },
    { 0,0,1 },
    { 1,1,1 },
  };

  std::vector<CompSprite3D*> sprites;

  // Sprite3D専用 頂点シェーダー
  Microsoft::WRL::ComPtr<ID3D11VertexShader> defaultVS = nullptr;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> billboardAxisVS = nullptr;
  Microsoft::WRL::ComPtr<ID3D11VertexShader> billboardAllVS = nullptr;

  // 定数バッファ( 0番を使用 )
  struct ConstantBuffer
  {
    DirectX::XMFLOAT4X4 worldMatrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

    DirectX::XMINT3 billboardData;
    int dummy;
  };
  Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer = nullptr;

};
