#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Component.h"
#include "Math/OriginMath.h"
#include "Graphics/ShaderStaties.h"

class CompParticleEmitter :public Component, public std::enable_shared_from_this<CompParticleEmitter>
{
public:
  // パーティクルの更新に使用する定数バッファ ( b13 )
  struct EmitterData
  {
    DirectX::XMFLOAT3 emitPosition = { 0,0,0 }; // パーティクルの生成位置
    float gravityModifier = 0.0f;   // 重力の影響を受ける

    float lifeTime = 1.0f;          // パーティクルの生存時間
    float emissivePower = 0.0f;     // 発光の強さ
    float spawnRandScale = 1.0f;    // 生成位置のランダムの大きさ
    float dummy = {};               // パディング

    DirectX::XMFLOAT3 moveVec = {}; // 進行方向
    float dummy2 = {};              // パディング

    float vecRandScale = {};  // 進行方向のランダムの大きさ
    float startSize = 1.0f;   // 生成時の大きさ
    float endSize = 1.0f;     // 消滅時の大きさ
    float dummy3 = {};        // パディング

    float particleSpeed = 1.0f;   // パーティクルの速度
    float speedRandScale = 1.0f;  // 速度のランダムの大きさ
    float startSpeedRate = 1.0f;  // 生成時の速さの倍率
    float endSpeedRate = 1.0f;    // 消滅時の速さの倍率

    DirectX::XMFLOAT4 startColor = { 1,1,1,1 }; // 生成時の色
    DirectX::XMFLOAT4 endColor = { 1,1,1,1 };   // 消滅時の色
  };

public:
  CompParticleEmitter(int max, const char* texturePath, const char* computeCSO);

  void Start()override;

  void Update(const float& elapsedTime);

  void EmitParticle(ID3D11DeviceContext* dc);

  void UpdateParticle(ID3D11DeviceContext* dc);

  void OnDestroy()override;

  void DrawImGui()override;

  void Play() { simulateTimer = 0; }

  void BufferAssign(ID3D11DeviceContext* dc, UINT initialCount = -1);
  void BufferUnassign(ID3D11DeviceContext* dc);

  // 以下ゲッター・セッター
  const char* GetName()const override { return "Particle"; };

  UINT GetMaxParticles()const { return maxParticles; }
  UINT GetParticleCount()const { return particleCount; }
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GetTexture() { return texture; }
  Microsoft::WRL::ComPtr<ID3D11ComputeShader>       GetEmitCompute() { return emitCompute; }
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  GetParticleSRV() { return particleBufferSRV; }
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetParticleUAV() { return particleBufferUAV; }
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetPoolUAV() { return particlePoolBufferUAV; }
  Microsoft::WRL::ComPtr<ID3D11Buffer>              GetParticleBuffer() { return particleBuffer; }

  void SetIsLoop(bool value) { isLoop = value; }
  void SetDuration(float value) { duration = value; }
  void SetEmitInterval(float value) { emitInterval = value; }
  void SetEmitRate(int value) { emitRate = value; }
  int  GetEmitRate() { return emitRate; }

  const BS_MODE GetBsMode()const { return bsMode; }

  void SetBsMode(const BS_MODE& mode) { bsMode = mode; }

  EmitterData emitterData;

private:
  void UpdateParticleCount(ID3D11DeviceContext* dc);

private:
  UINT maxParticles = 0;  // 最大数
  UINT particleCount = 0; // 生成できるパーティクルの数 ( 死亡しているパーティクル )

  // 描画ステート
  BS_MODE bsMode = BS_MODE::ADD;

  bool  isLoop = true;        // パーティクルの生成を無限に行う
  float duration = 0.0f;      // isLoopがFalseの際のパーティクルの生成期間
  float simulateTimer = 1.0f; // 更新総時間 ( duration用変数 )
  float emitInterval = 0.5f;  // パーティクル生成の間隔
  float emitTimer = 0.0f;     // パーティクル生成の時間 ( EmitInterval用変数 )
  bool  emitFlag = 0.0f;      // パーティクルを生成するフラグ
  UINT  emitRate = 1;         // 1回のパーティクルの生成数 ( THREAD_NUM_X * emitRate )

  // パーティクルのスプライト
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

  // パーティクル生成コンピュートシェーダー
  Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitCompute;

  // 実体が入るバッファー
  Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUAV;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSRV;

  // パーティクルの管理をするバッファー
  Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolBuffer;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolBufferUAV;

  inline static Microsoft::WRL::ComPtr<ID3D11ComputeShader> initialCompute;
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> particleCountBuffer;
};


