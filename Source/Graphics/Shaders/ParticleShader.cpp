#include "ParticleShader.h"
#include "Graphics/Graphics.h"

ParticleShader::ParticleShader()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // ���_�V�F�[�_�[
  createVsFromCso(device, "Shader/ParticleVS.cso", VS.GetAddressOf());

  // �W�I���g���V�F�[�_�[
  createGsFromCso(device, "Shader/ParticleGS.cso", GS.GetAddressOf());

  // �s�N�Z���V�F�[�_�[
  createPsFromCso(device, "Shader/ParticlePS.cso", PS.GetAddressOf());

}

void ParticleShader::Begin(ID3D11DeviceContext* dc)
{
  Activate(dc);
  dc->IASetInputLayout(NULL);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
  dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
}

void ParticleShader::Draw(ID3D11DeviceContext* dc, CompParticleEmitter* particle)
{
  SetUpStates(dc, particle->GetBsMode(), DS_MODE::ZT_ON_ZW_OFF, RS_MODE::SOLID_CULL_NONE);

  dc->PSSetShaderResources(0, 1, particle->GetTexture().GetAddressOf());
  dc->GSSetShaderResources(0, 1, particle->GetParticleSRV().GetAddressOf());

  dc->Draw(static_cast<UINT>(particle->GetMaxParticles()), 0);
}

HRESULT ParticleShader::createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertex_shader)
{
  return ShaderBase::createVsFromCso(device, csoName, VS.GetAddressOf(), nullptr, 0, 0);
}
