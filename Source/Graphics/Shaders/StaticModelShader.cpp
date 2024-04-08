#include "StaticModelShader.h"
#include "Graphics/Graphics.h"
#include "System/misc.h"

StaticModelShader::StaticModelShader(const char* vertexPath, const char* pixelPath)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // ���_�V�F�[�_�[
  {
    HRESULT hr = createVsFromCso(device, vertexPath, VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �s�N�Z���V�F�[�_�[
  if (pixelPath != nullptr)
  {
    HRESULT hr = createPsFromCso(device, pixelPath, PS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void StaticModelShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
  Activate(dc);
  dc->IASetInputLayout(inputLayout.Get());
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  SetUpStates(dc, BS_MODE::ALPHA, DS_MODE::ZT_ON_ZW_ON, RS_MODE::SOLID_CULL_BACK);
}

void StaticModelShader::Draw(ID3D11DeviceContext* dc, const StaticModelResource* resource, const int& drawVolume)
{
  uint32_t stride{ sizeof(StaticModelResource::ParticleData) };
  uint32_t offset{ 0 };

  for (int i = 0; i < resource->GetMaterials().size(); ++i)
  {
    // ���_����GPU�ɑ��M
    dc->IASetVertexBuffers(0, 1, resource->GetVertexBuffers().at(i).GetAddressOf(), &stride, &offset);

    // �e�N�X�`�����̑��M
    ID3D11ShaderResourceView* srvs[]{
      resource->GetMaterials().at(i).diffuse.Get(),
      resource->GetMaterials().at(i).normal.Get(),
      resource->GetMaterials().at(i).metallic.Get(),
      resource->GetMaterials().at(i).roughness.Get(),
      resource->GetMaterials().at(i).ambientOcclusion.Get(),
      resource->GetMaterials().at(i).emissive.Get()
    };
    dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

    // �h���[�R�[��
    D3D11_BUFFER_DESC bufferDesc{};
    resource->GetVertexBuffers().at(i)->GetDesc(&bufferDesc);
    dc->DrawInstanced(bufferDesc.ByteWidth / stride, drawVolume, 0, 0);
  }
}

HRESULT StaticModelShader::createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader)
{
  // ���̓��C�A�E�g
  D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

    // 4x4�̃��[���h�s��
    { "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    { "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
  };

  // ���_�V�F�[�_�[���쐬
  return ShaderBase::createVsFromCso(device, csoName, vertexShader, inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
}
