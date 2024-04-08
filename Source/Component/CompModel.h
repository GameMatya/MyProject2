#pragma once

#include "CompModelBase.h"
#include "ModelAnimator.h"
#include "Graphics/ModelResource.h"

// ���f��
class CompModel : public CompModelBase
{
public:
  struct Node // ( �{�[�� )
  {
    const char* name;   // �m�[�h ( �{�[�� ) �̖��O
    int         index;  // �m�[�h�̔ԍ�
    Node*       parent; // �e�m�[�h ( �e�{�[�� ) �̃|�C���^
    DirectX::XMFLOAT3	scale; // �傫��
    DirectX::XMFLOAT4	rotate; // ��]
    DirectX::XMFLOAT3	translate; // �ʒu
    DirectX::XMMATRIX localTransform; // ���[�J���s��
    DirectX::XMMATRIX worldTransform; // ���[���h�s��
    DirectX::XMMATRIX oldWorldTransform; // �O�̃t���[���̃��[���h�s��

    std::vector<Node*>	children; // �q�m�[�h ( �q�{�[�� ) �̃R���e�i
  };

public:
  CompModel(const char* filepath, ModelRenderer* modelRenderer);
  ~CompModel() {}

  // ���O�擾
  const char* GetName() const override { return "ModelComponent"; };

  void Start()override { gameObject.lock()->SetHaveModel(); }

  void Update(const float& elapsedTime)override;

  void OnDestroy()override;

  void DrawImGui()override { animator.DrawImGui(); }

  // ModelResource::Collision����ɓ����蔻���ǉ�
  void AddCompCollisions()override;

  // ������J�����O
  void FrustumCulling();

  // �m�[�h����
  Node* FindNode(const char* name);
  // �m�[�h���X�g�擾
  const std::vector<Node>& GetNodes() const { return nodes; }

  // ���\�[�X�擾
  const ModelResource* GetResource() const { return resource.get(); }

private:
  // �s��v�Z
  void UpdateTransform(const DirectX::XMMATRIX& Transform);
  void SettingNodeCollision(Collider3D* collider, const ModelResourceBase::Collision::Collider& collision)override;

public:
  ModelAnimator animator; // �A�j���[�V�����Đ��N���X

private:
  std::shared_ptr<ModelResource>	resource; // 3D���f���̑S�����i�[
  ModelRenderer*    renderer = nullptr;
  std::vector<Node> nodes; // 3D���f���̃m�[�h ( �{�[�� ) ���i�[

  bool isCulling; // 3D���f�����J�����O�Ώۂ����i�[

  friend ModelAnimator;
};
