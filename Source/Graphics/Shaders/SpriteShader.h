#pragma once

#include <array>

#include "Shader.h"
#include "../RenderContext/RenderContext.h"
#include "Graphics/ShaderStaties.h"

class SpriteShader :public ShaderBase
{
public:
  struct Vertex
  {
    DirectX::XMFLOAT3	position = {};
    DirectX::XMFLOAT2	texcoord = {};
    DirectX::XMFLOAT4	color = { 1,1,1,1 };
  };

  // 頂点バッファの更新に使用
  using VertexResource = std::array<Vertex, 4>;

public:
  SpriteShader();
  SpriteShader(const char* vsFilepath, const char* psFilepath);

  // 描画開始
  virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc,
    const BS_MODE& bsMode = BS_MODE::ALPHA, const DS_MODE& dsMode = DS_MODE::ZT_ON_ZW_ON, const RS_MODE& rsMode = RS_MODE::SOLID_CULL_BACK);

  // 描画
  virtual void Draw(ID3D11DeviceContext* dc, const VertexResource* vertexResource, ID3D11ShaderResourceView** shaderResource);

  // 描画終了
  virtual void End(ID3D11DeviceContext* dc) { Inactivate(dc); };

  // シェーダーの再設定
  void SetPixelShader(ID3D11Device* device, const char* csoPath);
  void SetVertexShader(ID3D11Device* device, const char* csoPath);

  HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertex_shader)override;

protected:
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

};
