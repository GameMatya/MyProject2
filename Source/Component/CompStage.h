#pragma once

#include "Component.h"
#include "Graphics/StaticModelResource.h"
#include <vector>

// レイキャストの結果
struct HitResult
{
  DirectX::XMFLOAT3 position = { };   // レイとポリゴンの交点
  DirectX::XMFLOAT3 normal = { };     // 衝突したポリゴンの法線ベクトル
  float             distance = FLT_MAX;  // レイの始点から交点までの距離
};

class CompStage :public Component
{
public:
  CompStage(const char* filepath);

  // 名前取得
  const char* GetName() const { return "Stage"; };

  // 生成した際に一度だけ実行
  void Start()override;

  // GUI描画
  void DrawImGui()override;

  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hitResult = nullptr);

private:
  bool IsInsideBox(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

  bool RayVsBox(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

private:
  std::shared_ptr<StaticModelResource> resource = nullptr;

  std::vector<ModelResourceBase::Collision::PolygonGroupe> groups;

};