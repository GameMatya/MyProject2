#include "SpriteShader.h"
#include "Graphics/Graphics.h"
#include "System/misc.h"

SpriteShader::SpriteShader() :ShaderBase()
{
  // ��x�����ʂ�
  if (vertexBuffer == nullptr) {
    ID3D11Device* device = Graphics::Instance().GetDevice();

    // ���_�o�b�t�@�̐���
    {
      // ���_�o�b�t�@���쐬���邽�߂̐ݒ�I�v�V����
      D3D11_BUFFER_DESC bufferDesc = {};
      bufferDesc.ByteWidth = sizeof(Vertex) * 4;
      bufferDesc.Usage = D3D11_USAGE_DEFAULT;
      bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;
      bufferDesc.StructureByteStride = 0;

      // ���_�o�b�t�@�I�u�W�F�N�g�̐���
      HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, vertexBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }
}

SpriteShader::SpriteShader(const char* vsFilepath, const char* psFilepath) :SpriteShader()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // ���_�V�F�[�_�[
  {
    HRESULT hr = createVsFromCso(device, vsFilepath, VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �s�N�Z���V�F�[�_�[
  {
    HRESULT hr = createPsFromCso(device, psFilepath, PS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void SpriteShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc,
  const BS_MODE& bsMode, const DS_MODE& dsMode, const RS_MODE& rsMode)
{
  Activate(dc);
  dc->IASetInputLayout(inputLayout.Get());

  dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  SetUpStates(dc, bsMode, dsMode, rsMode);
}

void SpriteShader::Draw(ID3D11DeviceContext* dc, const VertexResource* vertexResource, ID3D11ShaderResourceView** shaderResource)
{
  // ���_�o�b�t�@�X�V
  dc->UpdateSubresource(vertexBuffer.Get(), 0, 0, vertexResource, 0, 0);

  // �`��
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  dc->PSSetShaderResources(0, 1, shaderResource);
  dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
  dc->Draw(4, 0);
}

void SpriteShader::SetPixelShader(ID3D11Device* device, const char* csoPath)
{
  HRESULT hr = createPsFromCso(device, csoPath, PS.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void SpriteShader::SetVertexShader(ID3D11Device* device, const char* csoPath)
{
  HRESULT hr = createVsFromCso(device, csoPath, VS.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

HRESULT SpriteShader::createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader)
{
  // ���̓��C�A�E�g
  D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
  {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  // ���_�V�F�[�_�[���쐬
  return ShaderBase::createVsFromCso(device, cso_name, vertex_shader, inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
}
