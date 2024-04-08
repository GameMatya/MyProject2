#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Component.h"
#include "Math/OriginMath.h"
#include "Graphics/ShaderStaties.h"

class CompParticleEmitter :public Component, public std::enable_shared_from_this<CompParticleEmitter>
{
public:
  // �p�[�e�B�N���̍X�V�Ɏg�p����萔�o�b�t�@ ( b13 )
  struct EmitterData
  {
    DirectX::XMFLOAT3 emitPosition = { 0,0,0 }; // �p�[�e�B�N���̐����ʒu
    float gravityModifier = 0.0f;   // �d�͂̉e�����󂯂�

    float lifeTime = 1.0f;          // �p�[�e�B�N���̐�������
    float emissivePower = 0.0f;     // �����̋���
    float spawnRandScale = 1.0f;    // �����ʒu�̃����_���̑傫��
    float dummy = {};               // �p�f�B���O

    DirectX::XMFLOAT3 moveVec = {}; // �i�s����
    float dummy2 = {};              // �p�f�B���O

    float vecRandScale = {};  // �i�s�����̃����_���̑傫��
    float startSize = 1.0f;   // �������̑傫��
    float endSize = 1.0f;     // ���Ŏ��̑傫��
    float dummy3 = {};        // �p�f�B���O

    float particleSpeed = 1.0f;   // �p�[�e�B�N���̑��x
    float speedRandScale = 1.0f;  // ���x�̃����_���̑傫��
    float startSpeedRate = 1.0f;  // �������̑����̔{��
    float endSpeedRate = 1.0f;    // ���Ŏ��̑����̔{��

    DirectX::XMFLOAT4 startColor = { 1,1,1,1 }; // �������̐F
    DirectX::XMFLOAT4 endColor = { 1,1,1,1 };   // ���Ŏ��̐F
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

  // �ȉ��Q�b�^�[�E�Z�b�^�[
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
  UINT maxParticles = 0;  // �ő吔
  UINT particleCount = 0; // �����ł���p�[�e�B�N���̐� ( ���S���Ă���p�[�e�B�N�� )

  // �`��X�e�[�g
  BS_MODE bsMode = BS_MODE::ADD;

  bool  isLoop = true;        // �p�[�e�B�N���̐����𖳌��ɍs��
  float duration = 0.0f;      // isLoop��False�̍ۂ̃p�[�e�B�N���̐�������
  float simulateTimer = 1.0f; // �X�V������ ( duration�p�ϐ� )
  float emitInterval = 0.5f;  // �p�[�e�B�N�������̊Ԋu
  float emitTimer = 0.0f;     // �p�[�e�B�N�������̎��� ( EmitInterval�p�ϐ� )
  bool  emitFlag = 0.0f;      // �p�[�e�B�N���𐶐�����t���O
  UINT  emitRate = 1;         // 1��̃p�[�e�B�N���̐����� ( THREAD_NUM_X * emitRate )

  // �p�[�e�B�N���̃X�v���C�g
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

  // �p�[�e�B�N�������R���s���[�g�V�F�[�_�[
  Microsoft::WRL::ComPtr<ID3D11ComputeShader> emitCompute;

  // ���̂�����o�b�t�@�[
  Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUAV;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSRV;

  // �p�[�e�B�N���̊Ǘ�������o�b�t�@�[
  Microsoft::WRL::ComPtr<ID3D11Buffer> particlePoolBuffer;
  Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlePoolBufferUAV;

  inline static Microsoft::WRL::ComPtr<ID3D11ComputeShader> initialCompute;
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> particleCountBuffer;
};


