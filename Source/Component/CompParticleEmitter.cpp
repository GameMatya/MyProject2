#include "CompParticleEmitter.h"
#include "Graphics/Graphics.h"
#include "Graphics/texture.h"
#include "Graphics/ParticleManager.h"
#include "Graphics/Shaders/ParticleShader.h"
#include "System/misc.h"

#include <algorithm>
#include <imgui.h>

CompParticleEmitter::CompParticleEmitter(int maxVolume, const char* texturePath, const char* computeCSO)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // �e�N�X�`���̓ǂݍ���
  Texture::LoadTexture(texturePath, device, texture.ReleaseAndGetAddressOf());

  // �p�[�e�B�N���̍ő吔���X���b�h���Ŋ���؂�鐔�ɂ���
  maxParticles = static_cast<size_t>(maxVolume / THREAD_NUM_X) * THREAD_NUM_X;
  particleCount = maxParticles;

  HRESULT hr{ S_OK };

  // �R���s���[�g�V�F�[�_�[����
  ShaderBase::createCsFromCso(device, computeCSO, emitCompute.ReleaseAndGetAddressOf());

  // �o�b�t�@�쐬
  {
    D3D11_BUFFER_DESC bufferDesc{};

    // �p�[�e�B�N���̎��̗p�o�b�t�@�[
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleShader::ParticleData) * particleCount);
    bufferDesc.StructureByteStride = sizeof(ParticleShader::ParticleData);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    hr = device->CreateBuffer(&bufferDesc, nullptr, particleBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    bufferDesc = {};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * particleCount);
    bufferDesc.StructureByteStride = sizeof(UINT);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    // �X�^�b�N�p�o�b�t�@�[
    hr = device->CreateBuffer(&bufferDesc, nullptr, particlePoolBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �r���[�쐬
  {
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    uavDesc.Buffer.Flags = 0;
    hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, particleBufferUAV.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
    hr = device->CreateUnorderedAccessView(particlePoolBuffer.Get(), &uavDesc, particlePoolBufferUAV.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementOffset = 0;
    srvDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    hr = device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, particleBufferSRV.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �o�b�t�@�������p�R���s���[�g�V�F�[�_�[�̍쐬
  if (initialCompute == nullptr) {
    hr = ShaderBase::createCsFromCso(device, "Shader/ParticleInitCS.cso", initialCompute.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t));
    bufferDesc.StructureByteStride = sizeof(uint32_t);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU����ǂݎ��̂݉\
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, nullptr, particleCountBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void CompParticleEmitter::Start()
{
  std::unique_lock<std::mutex> lock(Graphics::Instance().GetMutex());
  ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

  DirectX::XMFLOAT4 rotation = GetGameObject()->transform.rotation;
  if (Mathf::Dot(rotation, rotation) == 0) {
    GetGameObject()->transform.rotation.y = 1.0f;
  }

  // �}�l�[�W���[�ɓo�^
  ParticleManager::Instance().Register(shared_from_this());

  dc->CSSetShader(initialCompute.Get(), NULL, 0);

  BufferAssign(dc, 0);

  // �R���s���[�g�V�F�[�_�[�̎��s
  dc->Dispatch(maxParticles / THREAD_NUM_X, 1, 1);

  BufferUnassign(dc);
}

void CompParticleEmitter::Update(const float& elapsedTime)
{
  emitTimer -= elapsedTime;

  // �^�C�}�[��0�ȉ��Ȃ琶��
  emitFlag = (emitTimer <= 0);

  ObjectTransform& transform = GetGameObject()->transform;

  DirectX::XMFLOAT3 position = transform.position;
  DirectX::XMFLOAT3 angle = {
    transform.rotation.x,
    transform.rotation.y,
    transform.rotation.z
  };

  if (transform.GetParentObject().expired() == false) {
    DirectX::XMVECTOR Angle = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&angle), *transform.GetParentTransform());
    DirectX::XMStoreFloat3(&angle, Angle);

    DirectX::XMVECTOR Position = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), *transform.GetParentTransform());
    DirectX::XMStoreFloat3(&position, Position);
  }

  emitterData.emitPosition = position;
  emitterData.moveVec = angle;

  if (isLoop == true) return;

  // �p�[�e�B�N�������[�v����Ȃ��ꍇ
  simulateTimer += elapsedTime;

  // �o�ߎ��Ԃ��w�肳�ꂽ���Ԉȓ��Ȃ琶��
  emitFlag = (emitFlag == true) && (simulateTimer <= duration);
}

void CompParticleEmitter::EmitParticle(ID3D11DeviceContext* dc)
{
  // �����t���O��False�Ȃ牽�����Ȃ�
  if (emitFlag == false)return;
  emitTimer = emitInterval;

  // ����������p�[�e�B�N���̐����X���b�h�̔{����
  UINT numThreads = static_cast<UINT>(emitRate) * THREAD_NUM_X;

  // �������悤�Ƃ��Ă��鐔���g�p�\�ȃp�[�e�B�N���̐��𒴂��Ă����甭�������Ȃ�
  if (particleCount < numThreads)
  {
    return;
  }

  // �V�F�[�_�[�ݒ�
  dc->CSSetShader(emitCompute.Get(), NULL, 0);

  // �R���s���[�g�V�F�[�_�[�̎��s
  dc->Dispatch(emitRate, 1, 1);
}

void CompParticleEmitter::UpdateParticle(ID3D11DeviceContext* dc)
{
  // �R���s���[�g�V�F�[�_�[�̎��s
  dc->Dispatch(maxParticles / THREAD_NUM_X, 1, 1);

  // �������Ă���p�[�e�B�N���̐����X�V
  UpdateParticleCount(dc);
}

void CompParticleEmitter::OnDestroy()
{
  ParticleManager::Instance().Remove(shared_from_this());
}

void CompParticleEmitter::BufferAssign(ID3D11DeviceContext* dc, UINT initialCount)
{
  // �o�b�t�@�ݒ�
  ID3D11UnorderedAccessView* uavs[] = {
    particleBufferUAV.Get(),
    particlePoolBufferUAV.Get()
  };

  dc->CSSetUnorderedAccessViews(0, 1, particleBufferUAV.GetAddressOf(), nullptr);
  dc->CSSetUnorderedAccessViews(1, 1, particlePoolBufferUAV.GetAddressOf(), &initialCount);
}

void CompParticleEmitter::DrawImGui()
{
  ImGui::InputInt("Particles", (int*)&particleCount);

  if (ImGui::Button("Play")) {
    Play();
  }

  int bsID = static_cast<int>(bsMode);
  ImGui::Combo("Blend State", &bsID, "NONE\0ALPHA\0ADD\0SUB\0REPLACE\0MULTI\0LIGHT\0DARK\0SCREEN\0\0");
  bsMode = static_cast<BS_MODE>(bsID);

  if (ImGui::TreeNode("Emitter")) {
    ImGui::Checkbox("isLoop", &isLoop);
    ImGui::DragFloat("duration", &duration);
    ImGui::DragInt("emitRate", (int*)&emitRate, 1);
    emitRate = max(emitRate, 1);
    ImGui::DragFloat("emitInterval", &emitInterval, 0.0001f, 0.0001f, 30.0f);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Status")) {
    ImGui::DragFloat("gravityModifier", &emitterData.gravityModifier, 0.01f, 0.0f, 5.0f);
    ImGui::DragFloat("lifeTime", &emitterData.lifeTime, 0.001f, 0.001f, 10.0f);
    ImGui::DragFloat("emissivePower", &emitterData.emissivePower, 0.01f, 0.01f, 10.0f);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Position")) {
    ImGui::DragFloat("SpawnRandScale", &emitterData.spawnRandScale, 0.1f, 0);
    emitterData.spawnRandScale = max(emitterData.spawnRandScale, 0);

    ImGui::TreePop();
  }

  if (ImGui::TreeNode("MoveVec")) {
    ImGui::DragFloat("VecRandScale", &emitterData.vecRandScale, 0.1f, 0, 1.0f);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Size")) {
    ImGui::DragFloat("Start", &emitterData.startSize, 0.1f, 0);
    emitterData.startSize = max(emitterData.startSize, 0);
    ImGui::DragFloat("End", &emitterData.endSize, 0.1f, 0);
    emitterData.endSize = max(emitterData.endSize, 0);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Speed")) {
    ImGui::DragFloat("BaseSpeed", &emitterData.particleSpeed, 0.1f);
    ImGui::DragFloat("BaseRandScale", &emitterData.speedRandScale, 0.1f);
    ImGui::DragFloat("StartRate", &emitterData.startSpeedRate, 0.1f);
    ImGui::DragFloat("EndRate", &emitterData.endSpeedRate, 0.1f);
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Color")) {
    ImGui::ColorEdit4("Start", (float*)&emitterData.startColor);
    ImGui::ColorEdit4("End", (float*)&emitterData.endColor);
    ImGui::TreePop();
  }
}

void CompParticleEmitter::BufferUnassign(ID3D11DeviceContext* dc)
{
  // �o�b�t�@����
  ID3D11UnorderedAccessView* null_unordered_access_view[] = { nullptr,nullptr };
  dc->CSSetUnorderedAccessViews(0, 2, null_unordered_access_view, nullptr);
}

void CompParticleEmitter::UpdateParticleCount(ID3D11DeviceContext* dc)
{
  HRESULT hr{ S_OK };
  // pool�o�b�t�@�[��UAV���p�[�e�B�N���̃J�E���g�p�̃o�b�t�@�[�ɃR�s�[����
  dc->CopyStructureCount(particleCountBuffer.Get(), 0, particlePoolBufferUAV.Get());
  D3D11_MAPPED_SUBRESOURCE mappedSubResource{};

  // ���\�[�X����f�[�^��ǂݎ��
  hr = dc->Map(particleCountBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedSubResource);
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

  particleCount = { *static_cast<UINT*>(mappedSubResource.pData) }; // �ҋ@���̃p�[�e�B�N���̐�

  dc->Unmap(particleCountBuffer.Get(), 0);
}
