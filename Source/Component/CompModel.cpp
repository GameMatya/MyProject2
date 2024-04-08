#include "Graphics/Graphics.h"
#include "Graphics/ModelRenderer.h"
#include "System/ResourceManager.h"

#include "CompModel.h"
#include "CompTriggerCollision.h"
#include "CompCollision.h"

#include <string>

// コンストラクタ
CompModel::CompModel(const char* filepath, ModelRenderer* modelRenderer) :animator(this), renderer(modelRenderer)
{
  // リソース読み込み  ( ModelResourceで引数の3Dモデルを読み込む )
  resource = ResourceManager::Instance().LoadModelResource(filepath);

  // ノード
  const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

  // 取得した全ノードを処理
  nodes.resize(resNodes.size());
  for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
  {
    auto&& src = resNodes.at(nodeIndex); // 3Dモデルのノード情報
    auto&& dst = nodes.at(nodeIndex); // ノードを格納する為のコンテナ（この時点では中身は空）

    dst.name = src.name.c_str();
    dst.index = static_cast<int>(nodeIndex);
    dst.scale = src.scale;
    dst.rotate = src.rotate;
    dst.translate = src.translate;

    // ノード情報に親番号が入っていたら、
    // コンテナからその番号目のアドレスを取得しparent変数に格納 ( 親ノードを格納 )
    dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;

    if (dst.parent != nullptr) // 親ノード情報が格納されていたら	（自身は子ノード 確定）
    {
      // 親ノードのchildrenコンテナに自身を登録 ( 親子関係を作成 ) 
      dst.parent->children.emplace_back(&dst);
    }
  }

  // 単位行列を初期姿勢として設定
  UpdateTransform(DirectX::XMMatrixIdentity());

  // Rendererに登録
  modelRenderer->Register(this);
}

void CompModel::Update(const float& elapsedTime)
{
  gameObject.lock()->transform.UpdateTransform();

  // アニメーションの更新
  animator.UpdateAnimations(elapsedTime);

  // 姿勢行列の更新
  UpdateTransform(gameObject.lock()->transform.GetWorldTransform());
}

void CompModel::OnDestroy()
{
  renderer->Remove(this);
}

void CompModel::AddCompCollisions()
{
  ObjectTransform& transform = gameObject.lock()->transform;

  // オブジェクトの行列更新
  transform.UpdateTransform();
  UpdateTransform(transform.GetWorldTransform());

  AddCollisionsFromResource(resource.get());
}

// 姿勢行列計算
void CompModel::UpdateTransform(const DirectX::XMMATRIX& Transform)
{
  if (nodes.empty())return;

  for (Node& node : nodes)
  {
    node.oldWorldTransform = node.worldTransform;

    // ローカル行列算出
    DirectX::XMMATRIX s = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
    DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
    node.localTransform = s * r * t;

    // ワールド行列算出
    DirectX::XMMATRIX parentNodeTransform;
    if (node.parent != nullptr)
    {
      parentNodeTransform = node.parent->worldTransform;
    }
    else
    {
      parentNodeTransform = Transform;
    }

    node.worldTransform = node.localTransform * parentNodeTransform;
  }
}

void CompModel::SettingNodeCollision(Collider3D* collider, const ModelResourceBase::Collision::Collider& collision)
{
  collider->SetTransform(&nodes[collision.nodeId].worldTransform);
}

void CompModel::FrustumCulling()
{
}

// ノード検索
CompModel::Node* CompModel::FindNode(const char* name)
{
  // 全てのノードを総当たりで名前比較する
  for (int i = 0; i < nodes.size(); ++i)
  {
    // string型を使う方法
    {
      std::string name1 = nodes.at(i).name;
      std::string name2 = name;

      if (name1 == name2) {
        return &nodes.at(i);
      }
    }

  }

  // 見つからなかった
  return nullptr;

}
