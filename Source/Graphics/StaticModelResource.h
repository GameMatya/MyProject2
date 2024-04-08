#pragma once

#include "Graphics/ModelResourceBase.h"

#define MAX_STATIC_BATCH (800) // 1種類あたりのモデルの最大数

class StaticModelResource : public ModelResourceBase
{
public:
  struct ParticleData
  {
    DirectX::XMFLOAT3	position = { 0, 0, 0 };
    DirectX::XMFLOAT3	normal = { 0, 0, 0 };
    DirectX::XMFLOAT3	tangent = { 0, 0, 0 };
    DirectX::XMFLOAT2	texcoord = { 0, 0 };
    DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

public:
  StaticModelResource() {}
  virtual ~StaticModelResource() {}

  const std::vector<std::vector<ParticleData>>& GetVertices()const { return vertices; }
  const std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& GetVertexBuffers()const { return vertexBuffers; }

protected:
  // モデルセットアップ
  void BuildModel(ID3D11Device* device, const char* dirname);

  // デシリアライズ
  void Deserialize(const char* filename);

protected:
  std::vector<std::vector<ParticleData>> vertices;
  std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>	vertexBuffers;

};
