#pragma once

#include <memory>
#include <string>
#include <map>

class ModelResource;
class StaticModelResource;

// リソースマネージャー
class ResourceManager
{
private:
  ResourceManager() {}
  ~ResourceManager() {}

public:
  // 唯一のインスタンス取得
  static ResourceManager& Instance() {
    static ResourceManager instance;
    return instance;
  }

  // モデルリソース読み込み
  std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
  std::shared_ptr<StaticModelResource> LoadStaticResource(const char* filename);

private:
  using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>;
  using StaticMap = std::map<std::string, std::shared_ptr<StaticModelResource>>;
  
  // 通常モデル
  ModelMap models;
  // GPUインスタンシング適用モデル
  StaticMap statics;

};
