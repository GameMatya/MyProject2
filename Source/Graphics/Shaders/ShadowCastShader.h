#pragma once

#include "Graphics/Shaders/ModelShader.h"

class ShadowCastShader : public ModelShader
{
public:
  ShadowCastShader(ID3D11Device* device);

  void DrawSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, DirectX::XMFLOAT4 color = { 1,1,1,1 }) override;
};
