#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <WICTextureLoader.h>

#include <stb_image.h>

#include "ModelResource.h"
#include "System/misc.h"
#include "System/Logger.h"
#include "Graphics/texture.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResource::Node, 1)
CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
CEREAL_CLASS_VERSION(ModelResource::ParticleData, 1)
CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
CEREAL_CLASS_VERSION(ModelResource::CharacterData, 1)
CEREAL_CLASS_VERSION(ModelResource, 1)

// シリアライズ
namespace DirectX
{
  template<class Archive>
  void serialize(Archive& archive, XMINT2& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMUINT4& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z),
      cereal::make_nvp("w", v.w)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT2& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT3& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT4& v)
  {
    archive(
      cereal::make_nvp("x", v.x),
      cereal::make_nvp("y", v.y),
      cereal::make_nvp("z", v.z),
      cereal::make_nvp("w", v.w)
    );
  }

  template<class Archive>
  void serialize(Archive& archive, XMFLOAT4X4& m)
  {
    archive(
      cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
      cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
      cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
      cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
    );
  }
}


template<class Archive>
void ModelResource::Node::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(id),
    CEREAL_NVP(name),
    CEREAL_NVP(path),
    CEREAL_NVP(parentIndex),
    CEREAL_NVP(scale),
    CEREAL_NVP(rotate),
    CEREAL_NVP(translate)
  );
}

template<class Archive>
void ModelResource::Subset::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(startIndex),
    CEREAL_NVP(indexCount),
    CEREAL_NVP(materialIndex)
  );
}

template<class Archive>
void ModelResource::ParticleData::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(position),
    CEREAL_NVP(normal),
    CEREAL_NVP(tangent),
    CEREAL_NVP(texcoord),
    CEREAL_NVP(color),
    CEREAL_NVP(boneWeight),
    CEREAL_NVP(boneIndex)
  );
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(vertices),
    CEREAL_NVP(indices),
    CEREAL_NVP(subsets),
    CEREAL_NVP(nodeIndex),
    CEREAL_NVP(nodeIndices),
    CEREAL_NVP(offsetTransforms)
  );
}

template<class Archive>
void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(scale),
    CEREAL_NVP(rotate),
    CEREAL_NVP(translate)
  );
}

template<class Archive>
void ModelResource::Keyframe::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(seconds),
    CEREAL_NVP(nodeKeys)
  );
}

template<class Archive>
inline void ModelResource::Animation::Event::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(frames),
    CEREAL_NVP(type)
  );
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(name),
    CEREAL_NVP(secondsLength),
    CEREAL_NVP(keyframes),
    CEREAL_NVP(events)
  );
}

template<class Archive>
void ModelResource::CharacterData::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(waistHeight),
    CEREAL_NVP(pushPower),
    CEREAL_NVP(armNodes)
  );
}

// モデル構築
void ModelResource::BuildModel(ID3D11Device* device, const char* dirname)
{
  for (Material& material : materials)
  {
    // 相対パスの解決
    char filename[256];

    ::_makepath_s(filename, 256, nullptr, dirname, material.AlbedoFilename.c_str(), nullptr);
    // テクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.diffuse.GetAddressOf(), 0xFFFFFFFF);

    ::_makepath_s(filename, 256, nullptr, dirname, material.NormalFilename.c_str(), nullptr);
    // ノーマルマップテクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.normal.GetAddressOf(), 0xFFFF7E7E);

    ::_makepath_s(filename, 256, nullptr, dirname, material.MetalFilename.c_str(), nullptr);
    // メタリックテクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.metallic.GetAddressOf(), 0xFFFFFF0C);

    ::_makepath_s(filename, 256, nullptr, dirname, material.RoughnessFilename.c_str(), nullptr);
    // ラフネステクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.roughness.GetAddressOf(), 0xFFFFFF0C);

    ::_makepath_s(filename, 256, nullptr, dirname, material.AO_Filename.c_str(), nullptr);
    // 環境遮蔽テクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.ambientOcclusion.GetAddressOf(), 0xFFFF);

    ::_makepath_s(filename, 256, nullptr, dirname, material.EmissiveFilename.c_str(), nullptr);
    // 自己発光テクスチャ読み込み
    LoadTexture(device, filename, nullptr, true, material.emissive.GetAddressOf(), 0x000000);
  }

  for (Mesh& mesh : meshes)
  {
    // サブセット
    for (Subset& subset : mesh.subsets)
    {
      subset.material = &materials.at(subset.materialIndex);
    }

    // 頂点バッファ
    {
      D3D11_BUFFER_DESC bufferDesc = {};
      D3D11_SUBRESOURCE_DATA subresourceData = {};

      bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * mesh.vertices.size());
      bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
      bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;
      bufferDesc.StructureByteStride = 0;
      subresourceData.pSysMem = mesh.vertices.data();
      subresourceData.SysMemPitch = 0;
      subresourceData.SysMemSlicePitch = 0;

      HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // インデックスバッファ
    {
      D3D11_BUFFER_DESC bufferDesc = {};
      D3D11_SUBRESOURCE_DATA subresourceData = {};

      bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
      bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
      bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      bufferDesc.CPUAccessFlags = 0;
      bufferDesc.MiscFlags = 0;
      bufferDesc.StructureByteStride = 0;
      subresourceData.pSysMem = mesh.indices.data();
      subresourceData.SysMemPitch = 0; //Not use for index buffers.
      subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
      HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }
}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
  std::ifstream istream(filename, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive(
        CEREAL_NVP(nodes),
        CEREAL_NVP(meshes),
        CEREAL_NVP(animations),
        CEREAL_NVP(characterData)
      );
    }
    catch (...)
    {
      LOG("model deserialize failed.\n%s\n", filename);
      return;
    }
  }
  else
  {
    char buffer[256];
    sprintf_s(buffer, sizeof(buffer), "File not found > %s", filename);
    _ASSERT_EXPR_A(false, buffer);
  }
}

// ノードインデックスを取得する
int ModelResource::FindNodeIndex(NodeId nodeId) const
{
  int nodeCount = static_cast<int>(nodes.size());
  for (int i = 0; i < nodeCount; ++i)
  {
    if (nodes[i].id == nodeId)
    {
      return i;
    }
  }
  return -1;
}
