#include <stdlib.h>
#include <fstream>
#include <functional>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <WICTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "System/misc.h"
#include "System/Logger.h"
#include "Graphics/ModelResourceBase.h"
#include "Graphics/texture.h"

// CEREALバージョン定義
CEREAL_CLASS_VERSION(ModelResourceBase::Material, 1)
CEREAL_CLASS_VERSION(ModelResourceBase::Collision, 1)
CEREAL_CLASS_VERSION(ModelResourceBase, 1)

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
void serialize(Archive& archive, ModelResourceBase::Collision::VertexData& v)
{
  archive(
    cereal::make_nvp("keyID", v.keyID),
    cereal::make_nvp("vertexA", v.vertexA)
  );
}

template<class Archive>
void serialize(Archive& archive, ModelResourceBase::Collision::PolygonGroupe& g)
{
  archive(
    cereal::make_nvp("aabbCenter", g.aabbCenter),
    cereal::make_nvp("aabbRadius", g.aabbRadius),
    cereal::make_nvp("vertexDatas", g.vertexDatas)
  );
}

template<class Archive>
void serialize(Archive& archive, ModelResourceBase::Collision::Collider& c)
{
  archive(
    cereal::make_nvp("position", c.position),
    cereal::make_nvp("radius", c.radius),
    cereal::make_nvp("height", c.height),
    cereal::make_nvp("capsulePosition", c.capsulePosition),
    cereal::make_nvp("rotation", c.rotation),
    cereal::make_nvp("shape", c.shape),
    cereal::make_nvp("nodeId", c.nodeId)
  );
}


template<class Archive>
void ModelResourceBase::Material::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(name),
    CEREAL_NVP(AlbedoFilename),
    CEREAL_NVP(NormalFilename),
    CEREAL_NVP(MetalFilename),
    CEREAL_NVP(RoughnessFilename),
    CEREAL_NVP(AO_Filename),
    CEREAL_NVP(EmissiveFilename),
    CEREAL_NVP(color),
    CEREAL_NVP(shaderID)
  );
}

template<class Archive>
void ModelResourceBase::Collision::serialize(Archive& archive, int version)
{
  archive(
    CEREAL_NVP(boundCenter),
    CEREAL_NVP(boundRadius),
    CEREAL_NVP(polygonGroupes),
    CEREAL_NVP(collisions),
    CEREAL_NVP(triggers),
    CEREAL_NVP(myTag),
    CEREAL_NVP(targetTag)
  );
}

// 読み込み
void ModelResourceBase::Load(ID3D11Device* device, const char* filename)
{
  // ディレクトリパス取得
  char drive[32], dir[256], dirname[256];
  ::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
  ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

  // デシリアライズ
  MaterialDeserialize(filename);
  CollisionDeserialize(filename);
  Deserialize(filename);

  // モデル構築
  BuildModel(device, dirname);
}

// テクスチャ読み込み
void ModelResourceBase::LoadTexture(ID3D11Device* device, const char* filename, const char* suffix, bool isDummy, ID3D11ShaderResourceView** srv, UINT dummyColor)
{
  // パスを分解
  char drive[256], dirname[256], fname[256], ext[256];
  ::_splitpath_s(filename, drive, sizeof(drive), dirname, sizeof(dirname), fname, sizeof(fname), ext, sizeof(ext));

  // 末尾文字を追加
  if (suffix != nullptr)
  {
    std::string find = fname;
    int dist = (int)find.rfind("_");

    strncpy_s(fname, fname, dist + 1);

    find = find.substr(0, dist);
    ::strcat_s(fname, sizeof(fname), suffix);
  }
  // パスを結合
  char filepath[256];
  ::_makepath_s(filepath, 256, drive, dirname, fname, ext);

  // テクスチャの読み込み
  HRESULT hr = Texture::LoadTexture(filepath, device, srv, nullptr, isDummy, dummyColor);
  assert(SUCCEEDED(hr) && "テクスチャ画像読み込み失敗(ModelResource)");

}

void ModelResourceBase::MaterialDeserialize(const char* filename)
{
  std::string path = filename;
  path.erase(path.rfind('.') + 1);
  path += "material";

  std::ifstream istream(path.c_str());
  if (istream.is_open())
  {
    cereal::JSONInputArchive archive(istream);

    try
    {
      archive(
        CEREAL_NVP(materials)
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

void ModelResourceBase::CollisionDeserialize(const char* filename)
{
  std::string path = filename;
  path.erase(path.rfind('.') + 1);
  path += "collision";

  std::ifstream istream(path.c_str(), std::ios::binary);
  if (istream.is_open())
  {
    cereal::BinaryInputArchive archive(istream);

    try
    {
      archive(
        CEREAL_NVP(collision)
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
