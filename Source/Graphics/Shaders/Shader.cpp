#include "Shader.h"
#include "Graphics/Graphics.h"
#include "Camera/Camera.h"
#include "System/misc.h"
#include <assert.h>

ShaderBase::ShaderBase()
{
  if (sceneConstantBuffer == nullptr) {
    Graphics& graphics = Graphics::Instance();

    // �萔�o�b�t�@�̍쐬
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(CbScene);
    desc.StructureByteStride = 0;

    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    std::unique_lock<std::mutex> lock(graphics.GetMutex());
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // �萔�o�b�t�@�ݒ�
    dc->VSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
    dc->GSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
    dc->HSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
    dc->CSSetConstantBuffers(5, 1, sceneConstantBuffer.GetAddressOf());
  }
}

// �萔�o�b�t�@�X�V
void ShaderBase::UpdateSceneConstant(ID3D11DeviceContext* dc, const float& elapsedTime, const float& totalTime)
{
  CbScene cbScene;
  Camera& camera = Camera::Instance();

  cbScene.view = camera.GetView();
  cbScene.projection = camera.GetProjection();
  DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&cbScene.view);
  DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&cbScene.projection);
  DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
  DirectX::XMStoreFloat4x4(&cbScene.inverseView, DirectX::XMMatrixInverse(nullptr, V));
  DirectX::XMStoreFloat4x4(&cbScene.inverseProjection, DirectX::XMMatrixInverse(nullptr, P));

  cbScene.cameraPosition = camera.GetEye();
  cbScene.cameraScope.x = camera.GetNearZ();
  cbScene.cameraScope.y = camera.GetFarZ();

  cbScene.time.x = totalTime;
  cbScene.time.y = elapsedTime;

  dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
}

//------------------------------------------------
//	 �L����
//------------------------------------------------
void ShaderBase::Activate(ID3D11DeviceContext* dc)
{
  dc->VSSetShader(VS.Get(), NULL, 0);
  dc->HSSetShader(HS.Get(), NULL, 0);
  dc->DSSetShader(DS.Get(), NULL, 0);
  dc->GSSetShader(GS.Get(), NULL, 0);
  dc->PSSetShader(PS.Get(), NULL, 0);
}

//------------------------------------------------
//	 ������
//------------------------------------------------
void ShaderBase::Inactivate(ID3D11DeviceContext* dc)
{
  dc->IASetInputLayout(nullptr);

  dc->VSSetShader(NULL, NULL, 0);
  dc->HSSetShader(NULL, NULL, 0);
  dc->DSSetShader(NULL, NULL, 0);
  dc->GSSetShader(NULL, NULL, 0);
  dc->PSSetShader(NULL, NULL, 0);

  ID3D11ShaderResourceView* srv[] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
  dc->VSSetShaderResources(0, ARRAYSIZE(srv), srv);
  dc->HSSetShaderResources(0, ARRAYSIZE(srv), srv);
  dc->DSSetShaderResources(0, ARRAYSIZE(srv), srv);
  dc->GSSetShaderResources(0, ARRAYSIZE(srv), srv);
  dc->PSSetShaderResources(0, ARRAYSIZE(srv), srv);
}

void ShaderBase::SetUpStates(ID3D11DeviceContext* dc, BS_MODE bs, DS_MODE ds, RS_MODE rs)
{
  ShaderStates& staties = ShaderStates::Instance();

  const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  dc->OMSetBlendState(staties.getBlendState(bs), blend_factor, 0xFFFFFFFF);
  dc->OMSetDepthStencilState(staties.getDepthStencilState(ds), ds == DS_MODE::MASK ? 1 : 0);
  dc->RSSetState(staties.getRasterizerState(rs));
}

//------------------------------------------------
//	 CSO��ǂݍ���
//------------------------------------------------
HRESULT ShaderBase::createVsFromCso(
  ID3D11Device* device,
  const char* csoName,
  ID3D11VertexShader** vertexShader,
  ID3D11InputLayout** inputLayout,
  D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
  UINT numElements)
{
  // �A�z�z��ɓ���CSO�t�@�C�����������m�F
  auto it = vsMap.find(csoName);
  if (it != vsMap.end())
  {
    // �������ꍇ�͂����Ԃ�
    *vertexShader = it->second.vertexShader.Get();
    (*vertexShader)->AddRef();
    if (inputLayout) {
      *inputLayout = it->second.inputLayout.Get();
      (*inputLayout)->AddRef();
    }
    return S_OK;
  }

  // �V�K��CSO��ǂݍ���
  FILE* fp = nullptr;
  errno_t error;
  error = fopen_s(&fp, csoName, "rb");
  if (error != 0)assert("CSO File not found");

  // �t�@�C���̃T�C�Y�����߂�
  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertexShader);
  assert(SUCCEEDED(hr));

  if (inputLayout) {
    hr = device->CreateInputLayout(inputElementDesc, numElements, cso_data.get(), cso_sz, inputLayout);
    assert(SUCCEEDED(hr));
  }

  // �����CSO�̏���A�z�z��Ɋi�[ 
  if (inputLayout)
    vsMap.insert(std::make_pair(csoName, SetVertexShaderAndInputLayout(*vertexShader, *inputLayout)));
  else
    vsMap.insert(std::make_pair(csoName, SetVertexShaderAndInputLayout(*vertexShader, nullptr)));

  return hr;
}

HRESULT ShaderBase::createHsFromCso(ID3D11Device* device, const char* csoName, ID3D11HullShader** hullShader)
{
  FILE* fp = nullptr;
  errno_t error;

  error = fopen_s(&fp, csoName, "rb");
  assert("CSO File not found");

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreateHullShader(cso_data.get(), cso_sz, nullptr, hullShader);
  assert(SUCCEEDED(hr));

  return hr;
}

HRESULT ShaderBase::createDsFromCso(ID3D11Device* device, const char* csoName, ID3D11DomainShader** domainShader)
{
  FILE* fp = nullptr;
  errno_t error;

  error = fopen_s(&fp, csoName, "rb");
  assert("CSO File not found");

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreateDomainShader(cso_data.get(), cso_sz, nullptr, domainShader);
  assert(SUCCEEDED(hr));

  return hr;
}

HRESULT ShaderBase::createGsFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader)
{
  FILE* fp = nullptr;
  errno_t error;

  error = fopen_s(&fp, csoName, "rb");
  assert("CSO File not found");

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometryShader);
  assert(SUCCEEDED(hr));

  return hr;
}

HRESULT ShaderBase::createPsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
  // �A�z�z��ɓ���CSO�t�@�C�����������m�F
  auto it = psMap.find(csoName);
  if (it != psMap.end())
  {
    // �������ꍇ�͂����Ԃ�
    *pixelShader = it->second.Get();
    (*pixelShader)->AddRef();
    return S_OK;
  }

  // �V�K�ǂݍ���
  FILE* fp = nullptr;
  errno_t error;

  error = fopen_s(&fp, csoName, "rb");
  if (error != 0) assert("CSO File not found");

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixelShader);
  assert(SUCCEEDED(hr));

  return hr;
}

HRESULT ShaderBase::createCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** comShader)
{
  FILE* fp = nullptr;
  errno_t error;

  error = fopen_s(&fp, csoName, "rb");
  assert("CSO File not found");

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
  fread(cso_data.get(), cso_sz, 1, fp);
  fclose(fp);

  HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, comShader);
  assert(SUCCEEDED(hr));

  return hr;
}