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
    // ���_�̏��
    struct VertexData
    {
      UINT keyID = -1;
      UINT vertexA = -1; // �|���S�����\������ŏ��̒��_
    };
    // AABB�ŃO���[�v���������|���S��
    struct PolygonGroupe {
      DirectX::XMFLOAT3 aabbCenter = {};
      DirectX::XMFLOAT3 aabbRadius = {};

      // ���̃O���[�v�Ɋ܂܂�钸�_���
      std::vector<VertexData>	      vertexDatas;
    };
    // �����蔻��
    struct Collider {
      DirectX::XMFLOAT3 position = {};
      float             radius = 1;
      float             height = 1;
      DirectX::XMFLOAT4 rotation = {};
      // �J�v�Z���̐ڑ���
      DirectX::XMFLOAT3 capsulePosition = {};

      // �����蔻��̌`��
      COLLIDER_SHAPE_3D shape = COLLIDER_SHAPE_3D::NONE;

      // ���_�Ƃ���m�[�h�ԍ�
      int nodeId = -1;
    };

    // �o�E���f�B���O�{�b�N�X ( �t���X�^���J�����O�p )
    DirectX::XMFLOAT3 boundCenter = {};
    DirectX::XMFLOAT3 boundRadius = {};

    // ���b�V���̃O���[�v
    std::vector<PolygonGroupe> polygonGroupes;

    // �����o������
    std::vector<Collider> collisions;
    // �����o�������̔���
    std::vector<Collider> triggers;

    // ���g�̔���̃^�O
    COLLIDER_TAGS myTag;
    // ����Ώۂ̃^�O
    COLLIDER_TAGS targetTag;

    template<class Archive>
    void serialize(Archive& archive, int version);
  };

  // �e��f�[�^�擾
  const std::vector<Material>& GetMaterials() const { return materials; }
  const Collision& GetCollision() const { return collision; }

  // �ǂݍ���
  void Load(ID3D11Device* device, const char* filename);

protected:
  // ���f���Z�b�g�A�b�v
  virtual void BuildModel(ID3D11Device* device, const char* dirname) = 0;

  // �e�N�X�`���ǂݍ���
  void LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool isDummy, ID3D11ShaderResourceView** srv, UINT dummyColor = 0xFFFFFFFF);

  virtual void Deserialize(const char* filename) = 0;

  void MaterialDeserialize(const char* filename);

  void CollisionDeserialize(const char* filename);

protected:
  std::vector<Material>	  materials;
  Collision	              collision;

};
