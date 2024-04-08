#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>

#include "Math/OriginMath.h"
#include "Graphics/Shaders/ParticleShader.h"
#include "Component/CompParticleEmitter.h"

class ParticleManager
{
private:
  using CompPtr = std::weak_ptr<CompParticleEmitter>;

public:
  ~ParticleManager() { Clear(); }

  static ParticleManager& Instance() {
    static ParticleManager instance;
    return instance;
  }

  void Register(CompPtr particle);
  void Remove(CompPtr particle);
  void Clear() { particles.clear(); }

  void Update();
  void Render();

private:
  ParticleManager();

  void UpdateConstantBuffer(ID3D11DeviceContext* dc, CompParticleEmitter::EmitterData data);

private:
  std::vector<CompPtr> particles;

  std::unique_ptr<ParticleShader> particleShader = nullptr;

  Microsoft::WRL::ComPtr<ID3D11ComputeShader> updateCompute;
  Microsoft::WRL::ComPtr<ID3D11Buffer> emitterCB;

};


