#include "System/ResourceManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/ModelResource.h"
#include "Graphics/StaticModelResource.h"

// ���f�����\�[�X�ǂݍ���
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
  std::string name = filename;
  ModelMap::const_iterator it = models.find(name);
  if (it != models.end()) {

    // �����N��̎������m�F
    if (it->second.use_count() > 0)
    {
      return it->second;
    }
    else
    {
      models.erase(it);
    }
  }

  // �V�K���f�����\�[�X�̍쐬�E�ǂݍ���
  std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
  model.get()->Load(Graphics::Instance().GetDevice(), filename);

  // �}�b�v�ɓo�^
  models.emplace(filename, model);

  return model;
}

std::shared_ptr<StaticModelResource> ResourceManager::LoadStaticResource(const char* filename)
{
  std::string name = filename;
  StaticMap::const_iterator it = statics.find(name);
  if (it != statics.end()) {

    // �����N��̎������m�F
    if (it->second.use_count() > 0)
    {
      return it->second;
    }
    else
    {
      statics.erase(it);
    }
  }

  // �V�K���f�����\�[�X�̍쐬�E�ǂݍ���
  std::shared_ptr<StaticModelResource> model = std::make_shared<StaticModelResource>();
  model.get()->Load(Graphics::Instance().GetDevice(), filename);

  // �}�b�v�ɓo�^
  statics.emplace(filename, model);

  return model;
}
