#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Graphics.h"
#include "Component/CompStaticModel.h"

// Model�N���X��p�̕`��N���X
class StaticModelRenderer
{
private:
  using ResourcePtr = const StaticModelResource*;

public:
  StaticModelRenderer();
  ~StaticModelRenderer() { Clear(); }

  // ���f���̕`��
  void Render(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader);

  //	 �`�惂�f���̓o�^
  void Register(CompStaticModel* model);

  //	 �`�惂�f���̓o�^����
  void Remove(CompStaticModel* model);

  //	 �`�惂�f���o�^�S����
  void Clear();

  // ���f�����o�^����Ă��邩
  bool HasAnyModels() { return !resources.empty(); }

private:
  // �`�惊�X�g
  std::vector<ResourcePtr> resources;
  std::map<ResourcePtr, std::vector<GameObject*>> objectMap;

  Microsoft::WRL::ComPtr<ID3D11Buffer>        instanceBuffer;

};
