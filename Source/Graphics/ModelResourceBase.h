#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Component/CompTriggerCollision.h"

class ModelResourceBase
{
public:
  ModelResourceBase() {}
  virtual ~ModelResourceBase() {}

  struct Material
  {
    std::string			  name;
    std::string			  AlbedoFilename;
    std::string			  NormalFilename;
    std::string			  MetalFilename;
    std::string			  RoughnessFilename;
    std::string			  AO_Filename;
    std::string			  EmissiveFilename;
    DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
    int               shaderID = 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallic;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientOcclusion;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissive;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Collision
  {
    // 頂点の情報
    struct VertexData
    {
      UINT keyID = -1;
      UINT vertexA = -1; // ポリゴンを構成する最初の頂点
    };
    // AABBでグループ分けしたポリゴン
    struct PolygonGroupe {
      DirectX::XMFLOAT3 aabbCenter = {};
      DirectX::XMFLOAT3 aabbRadius = {};

      // このグループに含まれる頂点情報
      std::vector<VertexData>	      vertexDatas;
    };
    // 当たり判定
    struct Collider {
      DirectX::XMFLOAT3 position = {};
      float             radius = 1;
      float             height = 1;
      DirectX::XMFLOAT4 rotation = {};
      // カプセルの接続先
      DirectX::XMFLOAT3 capsulePosition = {};

      // 当たり判定の形状
      COLLIDER_SHAPE_3D shape = COLLIDER_SHAPE_3D::NONE;

      // 原点とするノード番号
      int nodeId = -1;
    };

    // バウンディングボックス ( フラスタムカリング用 )
    DirectX::XMFLOAT3 boundCenter = {};
    DirectX::XMFLOAT3 boundRadius = {};

    // メッシュのグループ
    std::vector<PolygonGroupe> polygonGroupes;

    // 押し出し判定
    std::vector<Collider> collisions;
    // 押し出し無しの判定
    std::vector<Collider> triggers;

    // 自身の判定のタグ
    COLLIDER_TAGS myTag;
    // 判定対象のタグ
    COLLIDER_TAGS targetTag;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  // 各種データ取得
  const std::vector<Material>& GetMaterials() const { return materials; }
  const Collision& GetCollision() const { return collision; }

  // 読み込み
  void Load(ID3D11Device* device, const char* filename);

protected:
  // モデルセットアップ
  virtual void BuildModel(ID3D11Device* device, const char* dirname) = 0;

  // テクスチャ読み込み
  void LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool isDummy, ID3D11ShaderResourceView** srv, UINT dummyColor = 0xFFFFFFFF);

  virtual void Deserialize(const char* filename) = 0;

  void MaterialDeserialize(const char* filename);

  void CollisionDeserialize(const char* filename);

protected:
  std::vector<Material>	  materials;
  Collision	              collision;

};
