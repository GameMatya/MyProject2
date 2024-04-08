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

#include "StaticModelResource.h"
#include "System/misc.h"
#include "System/Logger.h"
#include "Graphics/texture.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(StaticModelResource::ParticleData, 1)
CEREAL_CLASS_VERSION(StaticModelResource, 1)

// シリアライズ
namespace DirectX
{
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
void StaticModelResource::ParticleData::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(position),
    CEREAL_NVP(normal),
    CEREAL_NVP(tangent),
    CEREAL_NVP(texcoord),
    CEREAL_NVP(color)
  );
}

// モデル構築
void StaticModelResource::BuildModel(ID3D11Device* device, const char* dirname)
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

  // 頂点バッファの作成
  {
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    vertexBuffers.resize(materials.size());
    for (int i = 0; i < vertices.size(); ++i) {
      bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * vertices.at(i).size());
      subresourceData.pSysMem = vertices.at(i).data();

      hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffers[i].ReleaseAndGetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }
}

// デシリアライズ
void StaticModelResource::Deserialize(const char* filename)
{
  std::ifstream istream(filename, std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive(
        CEREAL_NVP(vertices)
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
