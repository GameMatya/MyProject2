#include <mutex>

#include "Graphics/Graphics.h"
#include "ModelShader.h"
#include "System/misc.h"

ModelShader::ModelShader()
{
  if (meshConstantBuffer == nullptr) {
    Graphics& graphics = Graphics::Instance();
    std::unique_lock<std::mutex> lock(graphics.GetMutex());

    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    HRESULT hr;

    // メッシュ用バッファ
    desc.ByteWidth = sizeof(CbMesh);
    hr = graphics.GetDevice()->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // サブセット用バッファ
    desc.ByteWidth = sizeof(CbSubset);

    hr = graphics.GetDevice()->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // 定数バッファ設定
    ID3D11Buffer* constantBuffers[] =
    {
      meshConstantBuffer.Get(),
      subsetConstantBuffer.Get()
    };
    graphics.GetDeviceContext()->VSSetConstantBuffers(6, ARRAYSIZE(constantBuffers), constantBuffers);
    graphics.GetDeviceContext()->PSSetConstantBuffers(6, ARRAYSIZE(constantBuffers), constantBuffers);
  }
}

ModelShader::ModelShader(const char* vsFilepath, const char* psFilepath) :ModelShader()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // 頂点シェーダー
  {
    HRESULT hr = createVsFromCso(device, vsFilepath, VS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // ピクセルシェーダー
  {
    HRESULT hr = createPsFromCso(device, psFilepath, PS.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void ModelShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
{
  Activate(dc);
  dc->IASetInputLayout(inputLayout.Get());

  SetUpStates(dc, BS_MODE::ALPHA, DS_MODE::ZT_ON_ZW_ON, RS_MODE::SOLID_CULL_BACK);
}

void ModelShader::SetBuffers(ID3D11DeviceContext* dc, const std::vector<CompModel::Node>& nodes, const ModelResource::Mesh& mesh)
{
  // メッシュ用定数バッファ更新
  CbMesh cbMesh;
  ::memset(&cbMesh, 0, sizeof(cbMesh));
  if (mesh.nodeIndices.size() > 0)
  {
    for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
    {
      DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
      DirectX::XMMATRIX boneTransform = offsetTransform * nodes.at(mesh.nodeIndices.at(i)).worldTransform;
      DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
    }
  }
  else
  {
    DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[0], nodes.at(mesh.nodeIndex).worldTransform);
  }
  dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

  UINT stride = sizeof(ModelResource::ParticleData);
  UINT offset = 0;
  dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
  dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

HRESULT ModelShader::createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader)
{
  // 入力レイアウト
  D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  // 頂点シェーダーを作成
  return ShaderBase::createVsFromCso(device, cso_name, vertex_shader, inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
}