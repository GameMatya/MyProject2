#pragma once

#include "ModelResourceBase.h"
#include <map>

enum class ANIMATION_EVENT {
  ATTACK,         // 攻撃の判定時間
  CANCEL_ACCEPT,  // アニメーションのキャンセル受付時間
  EFFECT_START,   // エフェクト発生時間
  INVINCIBLE,     // 無敵時間
  MAX
};

class ModelResource : public ModelResourceBase
{
public:
  using NodeId = UINT64;

  struct ParticleData
  {
    DirectX::XMFLOAT3	position = { 0, 0, 0 };
    DirectX::XMFLOAT3	normal = { 0, 0, 0 };
    DirectX::XMFLOAT3	tangent = { 0, 0, 0 };
    DirectX::XMFLOAT2	texcoord = { 0, 0 };
    DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };
    DirectX::XMFLOAT4	boneWeight = { 1, 0, 0, 0 };
    DirectX::XMUINT4	boneIndex = { 0, 0, 0, 0 };

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Node
  {
    NodeId				    id;
    std::string			  name;
    std::string			  path;
    int					      parentIndex;
    DirectX::XMFLOAT3	scale;
    DirectX::XMFLOAT4	rotate;
    DirectX::XMFLOAT3	translate;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Subset
  {
    UINT		  startIndex = 0;
    UINT		  indexCount = 0;
    int			  materialIndex = 0;

    Material* material = nullptr;

    // Sort用 : ShaderIDで比較
    bool operator<(const Subset& other) {
      return this->material->shaderID < other.material->shaderID;
    }

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Mesh
  {
    std::vector<ParticleData>						        vertices;
    std::vector<UINT>						          indices;
    std::vector<Subset>						        subsets;

    int										                nodeIndex;
    std::vector<int>						          nodeIndices;
    std::vector<DirectX::XMFLOAT4X4>		  offsetTransforms;

    Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct NodeKeyData
  {
    DirectX::XMFLOAT3	scale;
    DirectX::XMFLOAT4	rotate;
    DirectX::XMFLOAT3	translate;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Keyframe
  {
    float						seconds;
    std::vector<NodeKeyData>	nodeKeys;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct Animation
  {
    struct Event {
      DirectX::XMINT2 frames;
      ANIMATION_EVENT type;
      int             index;

      template<class Archive>
      void serialize(Archive& archive, int version);
    };
    std::string					    name;
    float						        secondsLength;
    std::vector<Keyframe>		keyframes;
    std::vector<Event>      events;
    bool                    rootMotionFlg = false;
    bool                    applyAxisUpFlg = false;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  struct CharacterData
  {
    struct RootMotionParam {
      float motionScale = 300.0f;
      std::vector<DirectX::XMFLOAT3> manualMovements;

      template<class Archive>
      void serialize(Archive& archive, int version);
    };

    float waistHeight = 5.0f;
    float pushPower = 10.0f;

    int spineNodeId = -1;
    int rootNodeId = -1;

    std::map<int,RootMotionParam> rootMotionParams;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

public:
  ModelResource() {}
  virtual ~ModelResource() {}

  void SetRootMotionScale(const int& animeId, const float& scale) { characterData.rootMotionParams[animeId].motionScale = scale; }
  
  // 各種データ取得
  const std::vector<Mesh>& GetMeshes() const { return meshes; }
  const std::vector<Node>& GetNodes() const { return nodes; }
  const std::vector<Animation>& GetAnimations() const { return animations; }
  const CharacterData& GetCharacterData() const { return characterData; }

#ifdef _DEBUG
  CharacterData& GetCharacterData_DebugOnly() { return characterData; }

  bool CharacterSerialize(const char* filename);

  std::string ResourcePath() { return resourcePath; }

#endif // _DEBUG

protected:
  // モデルセットアップ
  void BuildModel(ID3D11Device* device, const char* dirname);

  // デシリアライズ
  void Deserialize(const char* filename)override;

  // ノードインデックスを取得する
  int FindNodeIndex(NodeId nodeId) const;

  void AnimationDeserialize(const char* filename);

  void CharacterDeserialize(const char* filename);

protected:
  std::vector<Node>		    nodes;
  std::vector<Mesh>		    meshes;
  std::vector<Animation>	animations;
  CharacterData	          characterData;

#ifdef _DEBUG
  std::string resourcePath;

#endif
};
