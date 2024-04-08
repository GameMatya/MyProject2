#include "Graphics/Graphics.h"
#include "Graphics/ModelRenderer.h"
#include "System/ResourceManager.h"

#include "CompStaticModel.h"
#include "CompCollision.h"

CompStaticModel::CompStaticModel(const char* filepath, ModelRenderer* modelRenderer) :renderer(modelRenderer->GetStaticRenderer())
{
  // リソース読み込み  ( ModelResourceで引数の3Dモデルを読み込む )
  resource = ResourceManager::Instance().LoadStaticResource(filepath);
}

void CompStaticModel::Start()
{
  // Rendererに登録
  renderer->Register(this);
}

void CompStaticModel::OnDestroy()
{
  renderer->Remove(this);
}

void CompStaticModel::AddCompCollisions()
{
  // オブジェクトの行列更新
  gameObject.lock()->transform.UpdateTransform();

  AddCollisionsFromResource(resource.get());
}
