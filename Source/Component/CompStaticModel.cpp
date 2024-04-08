#include "Graphics/Graphics.h"
#include "Graphics/ModelRenderer.h"
#include "System/ResourceManager.h"

#include "CompStaticModel.h"
#include "CompCollision.h"

CompStaticModel::CompStaticModel(const char* filepath, ModelRenderer* modelRenderer) :renderer(modelRenderer->GetStaticRenderer())
{
  // ���\�[�X�ǂݍ���  ( ModelResource�ň�����3D���f����ǂݍ��� )
  resource = ResourceManager::Instance().LoadStaticResource(filepath);
}

void CompStaticModel::Start()
{
  // Renderer�ɓo�^
  renderer->Register(this);
}

void CompStaticModel::OnDestroy()
{
  renderer->Remove(this);
}

void CompStaticModel::AddCompCollisions()
{
  // �I�u�W�F�N�g�̍s��X�V
  gameObject.lock()->transform.UpdateTransform();

  AddCollisionsFromResource(resource.get());
}
