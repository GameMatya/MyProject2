#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <string>

#include "Component.h"
#include "Math/OriginMath.h"
#include "Graphics/ShaderStaties.h"
#include "Graphics/Texture.h"
#include "Graphics/Graphics.h"

// スプライト
class CompSpriteBase : public Component
{
public:
  CompSpriteBase(const char* filename);
  ~CompSpriteBase() {}

  // アフィン変換
  void CalcAffineTransform(const DirectX::XMFLOAT2& screenPosition);

  // GUI描画
  virtual void DrawImGui()override;

  void SetShaderID(const SHADER_ID_SPRITE& id) { shaderID = id; }

  Texture* GetTexture() { return texture.get(); }
  SpriteShader::VertexResource GetVertexResource() { return vertexResource; }

  const SHADER_ID_SPRITE GetShaderID() const { return shaderID; }

  const BS_MODE GetBsMode()const { return bsMode; }
  const DS_MODE GetDsMode()const { return dsMode; }
  const RS_MODE GetRsMode()const { return rsMode; }

  const float GetAngle() const { return angle; }
  const DirectX::XMFLOAT2 GetPivot() const { return pivot; }
  const DirectX::XMFLOAT2 GetTexPos() const { return texPos; }
  const DirectX::XMFLOAT2 GetTexSize() const { return texSize; }
  const DirectX::XMFLOAT4 GetColor() const { return color; }

  void SetBsMode(const BS_MODE& mode) { bsMode = mode; }
  void SetDsMode(const DS_MODE& mode) { dsMode = mode; }
  void SetRsMode(const RS_MODE& mode) { rsMode = mode; }

  void SetAngle(float degree) { angle = degree; }
  void SetPivot(DirectX::XMFLOAT2 newPivot) { pivot = newPivot; }
  void SetTexPos(DirectX::XMFLOAT2 newTexPos) { texPos = newTexPos; }
  void SetTexSize(DirectX::XMFLOAT2 newTexSize) { texSize = newTexSize; }
  void SetColor(DirectX::XMFLOAT4 newColor) { color = newColor; }

protected:
  std::unique_ptr<Texture> texture;
  SpriteShader::VertexResource vertexResource;

  // 使用するシェーダー
  SHADER_ID_SPRITE shaderID = SHADER_ID_SPRITE::DEFAULT;

  // 描画ステート
  BS_MODE bsMode = BS_MODE::ALPHA;
  DS_MODE dsMode = DS_MODE::ZT_ON_ZW_ON;
  RS_MODE rsMode = RS_MODE::SOLID_CULL_BACK;

  float             angle = 0.0f;
  DirectX::XMFLOAT2 pivot = {};
  DirectX::XMFLOAT2 texPos = {};
  DirectX::XMFLOAT2 texSize = {};
  DirectX::XMFLOAT4 color = { 1,1,1,1 };

};