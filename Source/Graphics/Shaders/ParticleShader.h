#pragma once

#include "Shader.h"
#include "../RenderContext/RenderContext.h"
#include "Graphics/ShaderStaties.h"
#include "Component/CompParticleEmitter.h"

static const int THREAD_NUM_X = 16;

class ParticleShader :public ShaderBase
{
public:
  // í∏ì_ç\ë¢ëÃ
  struct ParticleData {
    DirectX::XMFLOAT3 position = {};
    float							size = 1.0f;
    float							speed = 1.0f;
    DirectX::XMFLOAT3 moveVector = { 0,1,0 };
    float             gravityAccume = 0.0f;
    DirectX::XMFLOAT4 color = { 1,1,1,1 };
    float             emissivePower = 1.0f;
    float							lifeTimer = 10.0f;
    bool              isAlive = false;
  };

public:
  ParticleShader();

  // ï`âÊäJén
  void Begin(ID3D11DeviceContext* dc);

  // ï`âÊ
  void Draw(ID3D11DeviceContext* dc, CompParticleEmitter* particle);

  // ï`âÊèIóπ
  void End(ID3D11DeviceContext* dc) { Inactivate(dc); };

private:
  HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertex_shader)override;

};
