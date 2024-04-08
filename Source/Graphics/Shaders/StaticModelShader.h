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

  // �`��J�n
  virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc);

  // �`��
  virtual void Draw(ID3D11DeviceContext* dc, const StaticModelResource* resource, const int& drawVolume);

  // �`��I��
  virtual void End(ID3D11DeviceContext* dc) { Inactivate(dc); };

protected:
  // VS�p��CSO��ǂݍ��� ( �֐��̃��b�v )
  HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader)override;

};
