#pragma once

#include "CompModelBase.h"
#include "ModelAnimator.h"
#include "Graphics/ModelResource.h"

// モデル
class CompModel : public CompModelBase
{
public:
  struct Node // ( ボーン )
  {
    const char* name;   // ノード ( ボーン ) の名前
    int         index;  // ノードの番号
    Node*       parent; // 親ノード ( 親ボーン ) のポインタ
    DirectX::XMFLOAT3	scale; // 大きさ
    DirectX::XMFLOAT4	rotate; // 回転
    DirectX::XMFLOAT3	translate; // 位置
    DirectX::XMMATRIX localTransform; // ローカル行列
    DirectX::XMMATRIX worldTransform; // ワールド行列
    DirectX::XMMATRIX oldWorldTransform; // 前のフレームのワールド行列

    std::vector<Node*>	children; // 子ノード ( 子ボーン ) のコンテナ
  };

public:
  CompModel(const char* filepath, ModelRenderer* modelRenderer);
  ~CompModel() {}

  // 名前取得
  const char* GetName() const override { return "ModelComponent"; };

  void Start()override { gameObject.lock()->SetHaveModel(); }

  void Update(const float& elapsedTime)override;

  void OnDestroy()override;

  void DrawImGui()override { animator.DrawImGui(); }

  // ModelResource::Collisionを基に当たり判定を追加
  void AddCompCollisions()override;

  // 視錐台カリング
  void FrustumCulling();

  // ノード検索
  Node* FindNode(const char* name);
  // ノードリスト取得
  const std::vector<Node>& GetNodes() const { return nodes; }

  // リソース取得
  const ModelResource* GetResource() const { return resource.get(); }

private:
  // 行列計算
  void UpdateTransform(const DirectX::XMMATRIX& Transform);
  void SettingNodeCollision(Collider3D* collider, const ModelResourceBase::Collision::Collider& collision)override;

public:
  ModelAnimator animator; // アニメーション再生クラス

private:
  std::shared_ptr<ModelResource>	resource; // 3Dモデルの全情報を格納
  ModelRenderer*    renderer = nullptr;
  std::vector<Node> nodes; // 3Dモデルのノード ( ボーン ) を格納

  bool isCulling; // 3Dモデルがカリング対象かを格納

  friend ModelAnimator;
};
