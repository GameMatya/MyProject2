#pragma once

#include <memory>
#include <string>
#include <map>

class ModelResource;
class StaticModelResource;

// ���\�[�X�}�l�[�W���[
class ResourceManager
{
private:
  ResourceManager() {}
  ~ResourceManager() {}

public:
  // �B��̃C���X�^���X�擾
  static ResourceManager& Instance() {
    static ResourceManager instance;
    return instance;
  }

  // ���f�����\�[�X�ǂݍ���
  std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
  std::shared_ptr<StaticModelResource> LoadStaticResource(const char* filename);

private:
  using ModelMap = std::map<std::string, std::shared_ptr<ModelResource>>;
  using StaticMap = std::map<std::string, std::shared_ptr<StaticModelResource>>;
  
  // �ʏ탂�f��
  ModelMap models;
  // GPU�C���X�^���V���O�K�p���f��
  StaticMap statics;

};
