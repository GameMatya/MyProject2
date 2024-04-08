#pragma once

#include "Shader.h"
#include "Component/CompStaticModel.h"
#include "../RenderContext/RenderContext.h"
#include "Graphics/ShaderStaties.h"
#include "Math/OriginMath.h"

class StaticModelShader :public ShaderBase
{
public:
  StaticModelShader() {}
  StaticModelShader(const char* vertexPath,const char* pixelPath);
  virtual ~StaticModelShader() {}

  // 描画開始
  virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc);

  // 描画
  virtual void Draw(ID3D11DeviceContext* dc, const StaticModelResource* resource, const int& drawVolume);

  // 描画終了
  virtual void End(ID3D11DeviceContext* dc) { Inactivate(dc); };

protected:
  // VS用のCSOを読み込む ( 関数のラップ )
  HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader)override;

};
