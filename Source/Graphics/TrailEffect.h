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

  // トレイルの位置情報
  struct TrailData
  {
    DirectX::XMFLOAT3 head = {}; // トレイルの先端の位置
    DirectX::XMFLOAT3 tail = {}; // トレイルの末端の位置
    float lifeTimer = -1.0f;
  };

public:
  TrailEffect(const int& maxVolume, const float& lifeTime, const char* filepath);
  ~TrailEffect();

  void Set(const DirectX::XMFLOAT3& headPos, const DirectX::XMFLOAT3& tailPos);

  // 頂点配列の更新
  void Update();

  void Render(ID3D11DeviceContext* dc);

  void Clear();

  // スプライン補間の間隔 ( 0 ~ 1 )
  void SetVelvetyRate(const float& rate) { velvetyRate = rate; }

private:
  void Draw(ID3D11DeviceContext* dc);

  // 頂点バッファに設定するデータを更新
  void UpdateTrailData(const float& elapsedTime);

  // 頂点バッファに設定するデータを更新
  void UpdateVertexBufferData();

  // 頂点バッファ用コンテナにセット
  void AddVertex(const DirectX::XMVECTOR& position, const DirectX::XMFLOAT2& texcoord);
  void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord);

  // スプライン補間でトレイルの頂点数を増やす
  void AddSplineCurve(const float& amount, float& v);

private:
  // トレイルデータの最大数
  const int   MAX_TRAIL_DATA_VOLUME;
  // トレイルの寿命
  const float TRAIL_LIFE;

  // カーブの滑らかさ ( 滑らか : 0 ~ 1 : 粗い )
  float velvetyRate = 0.3f;

  // トレイル情報コンテナ
  std::deque<TrailData> trailDataCollection;

  // 頂点バッファ用データ
  std::vector<SpriteShader::Vertex> vertices;

  // トレイルエフェクトに使用するテクスチャ
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;

  Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

  inline static std::unique_ptr<SpriteShader> shader;
};
