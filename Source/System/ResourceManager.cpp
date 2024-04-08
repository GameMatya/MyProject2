#include "System/ResourceManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/ModelResource.h"
#include "Graphics/StaticModelResource.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
  std::string name = filename;
  ModelMap::const_iterator it = models.find(name);
  if (it != models.end()) {

    // リンク先の寿命を確認
    if (it->second.use_count() > 0)
    {
      return it->second;
    }
    else
    {
      models.erase(it);
    }
  }

  // 新規モデルリソースの作成・読み込み
  std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
  model.get()->Load(Graphics::Instance().GetDevice(), filename);

  // マップに登録
  models.emplace(filename, model);

  return model;
}

std::shared_ptr<StaticModelResource> ResourceManager::LoadStaticResource(const char* filename)
{
  std::string name = filename;
  StaticMap::const_iterator it = statics.find(name);
  if (it != statics.end()) {

    // リンク先の寿命を確認
    if (it->second.use_count() > 0)
    {
      return it->second;
    }
    else
    {
      statics.erase(it);
    }
  }

  // 新規モデルリソースの作成・読み込み
  std::shared_ptr<StaticModelResource> model = std::make_shared<StaticModelResource>();
  model.get()->Load(Graphics::Instance().GetDevice(), filename);

  // マップに登録
  statics.emplace(filename, model);

  return model;
}
