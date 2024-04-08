#include "ParticleManager.h"
#include "Graphics.h"
#include "System/misc.h"

ParticleManager::ParticleManager()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  particleShader = std::make_unique<ParticleShader>();

  HRESULT hr;

  // コンピュートシェーダー生成
  ShaderBase::createCsFromCso(device, "Shader/ParticleUpdateCS.cso", updateCompute.GetAddressOf());

  // エミッター用 定数バッファ
  D3D11_BUFFER_DESC desc;
  ::memset(&desc, 0, sizeof(desc));
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.StructureByteStride = 0;
  desc.ByteWidth = sizeof(CompParticleEmitter::EmitterData);
  hr = device->CreateBuffer(&desc, 0, emitterCB.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

  // 定数バッファ設定
  Graphics::Instance().GetDeviceContext()->CSSetConstantBuffers(6, 1, emitterCB.GetAddressOf());
}

void ParticleManager::Register(CompPtr particle)
{
#ifdef _DEBUG
  // 同じオブジェクトがすでに登録されていないか確認
  std::vector<CompPtr>::iterator it = std::find_if(particles.begin(), particles.end(), [&](const CompPtr& ptr1) {
    return ptr1.lock() == particle.lock();
    });

  assert(it == particles.end());
#endif // _DEBUG

  particles.emplace_back(particle);
}

void ParticleManager::Remove(CompPtr particle)
{
  // イテレータを取得する
  std::vector<CompPtr>::iterator it = std::find_if(particles.begin(), particles.end(), [&](const CompPtr& ptr1) {
    return ptr1.lock() == particle.lock();
    });

  particles.erase(it);
}

void ParticleManager::Update()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  // パーティクルの更新
  for (auto& particle : particles) {
    particle.lock()->BufferAssign(dc);
    UpdateConstantBuffer(dc, particle.lock()->emitterData);

    // パーティクル生成
    particle.lock()->EmitParticle(dc);

    // パーティクル更新
    dc->CSSetShader(updateCompute.Get(), NULL, 0);
    particle.lock()->UpdateParticle(dc);

    particle.lock()->BufferUnassign(dc);
  }

  // コンピュートシェーダー解除
  dc->CSSetShader(nullptr, NULL, 0);
}

void ParticleManager::Render()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  // シェーダー設定
  particleShader->Begin(dc);

  // 描画
  for (auto& particle : particles) {
    particleShader->Draw(dc, particle.lock().get());
  }

  particleShader->End(dc);
}

void ParticleManager::UpdateConstantBuffer(ID3D11DeviceContext* dc, CompParticleEmitter::EmitterData data)
{
  dc->UpdateSubresource(emitterCB.Get(), 0, 0, &data, 0, 0);
}
