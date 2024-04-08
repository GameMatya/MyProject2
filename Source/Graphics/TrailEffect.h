#pragma once

#include "Math/OriginMath.h"
#include "Component/CompSprite2D.h"
#include "Graphics/Shaders/SpriteShader.h"

#include <deque>
#include <vector>
#include <memory>
#include <wrl.h>

class TrailEffect
{
private:
  TrailEffect(const TrailEffect&) = delete;
  TrailEffect(TrailEffect&&)                 noexcept = delete;
  TrailEffect& operator=(TrailEffect const&) = delete;
  TrailEffect& operator=(TrailEffect&&)      noexcept = delete;

  // �g���C���̈ʒu���
  struct TrailData
  {
    DirectX::XMFLOAT3 head = {}; // �g���C���̐�[�̈ʒu
    DirectX::XMFLOAT3 tail = {}; // �g���C���̖��[�̈ʒu
    float lifeTimer = -1.0f;
  };

public:
  TrailEffect(const int& maxVolume, const float& lifeTime, const char* filepath);
  ~TrailEffect();

  void Set(const DirectX::XMFLOAT3& headPos, const DirectX::XMFLOAT3& tailPos);

  // ���_�z��̍X�V
  void Update();

  void Render(ID3D11DeviceContext* dc);

  void Clear();

  // �X�v���C����Ԃ̊Ԋu ( 0 ~ 1 )
  void SetVelvetyRate(const float& rate) { velvetyRate = rate; }

private:
  void Draw(ID3D11DeviceContext* dc);

  // ���_�o�b�t�@�ɐݒ肷��f�[�^���X�V
  void UpdateTrailData(const float& elapsedTime);

  // ���_�o�b�t�@�ɐݒ肷��f�[�^���X�V
  void UpdateVertexBufferData();

  // ���_�o�b�t�@�p�R���e�i�ɃZ�b�g
  void AddVertex(const DirectX::XMVECTOR& position, const DirectX::XMFLOAT2& texcoord);
  void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord);

  // �X�v���C����ԂŃg���C���̒��_���𑝂₷
  void AddSplineCurve(const float& amount, float& v);

private:
  // �g���C���f�[�^�̍ő吔
  const int   MAX_TRAIL_DATA_VOLUME;
  // �g���C���̎���
  const float TRAIL_LIFE;

  // �J�[�u�̊��炩�� ( ���炩 : 0 ~ 1 : �e�� )
  float velvetyRate = 0.3f;

  // �g���C�����R���e�i
  std::deque<TrailData> trailDataCollection;

  // ���_�o�b�t�@�p�f�[�^
  std::vector<SpriteShader::Vertex> vertices;

  // �g���C���G�t�F�N�g�Ɏg�p����e�N�X�`��
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;

  Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

  inline static std::unique_ptr<SpriteShader> shader;
};
