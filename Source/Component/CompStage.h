#pragma once

#include "Component.h"
#include "Graphics/StaticModelResource.h"
#include <vector>

// ���C�L���X�g�̌���
struct HitResult
{
  DirectX::XMFLOAT3 position = { };   // ���C�ƃ|���S���̌�_
  DirectX::XMFLOAT3 normal = { };     // �Փ˂����|���S���̖@���x�N�g��
  float             distance = FLT_MAX;  // ���C�̎n�_�����_�܂ł̋���
};

class CompStage :public Component
{
public:
  CompStage(const char* filepath);

  // ���O�擾
  const char* GetName() const { return "Stage"; };

  // ���������ۂɈ�x�������s
  void Start()override;

  // GUI�`��
  void DrawImGui()override;

  bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hitResult = nullptr);

private:
  bool IsInsideBox(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

  bool RayVsBox(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& positive, const DirectX::XMFLOAT3& negative);

private:
  std::shared_ptr<StaticModelResource> resource = nullptr;

  std::vector<ModelResourceBase::Collision::PolygonGroupe> groups;

};