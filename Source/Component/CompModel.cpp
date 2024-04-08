#include "Graphics/Graphics.h"
#include "Graphics/ModelRenderer.h"
#include "System/ResourceManager.h"

#include "CompModel.h"
#include "CompTriggerCollision.h"
#include "CompCollision.h"

#include <string>

// �R���X�g���N�^
CompModel::CompModel(const char* filepath, ModelRenderer* modelRenderer) :animator(this), renderer(modelRenderer)
{
  // ���\�[�X�ǂݍ���  ( ModelResource�ň�����3D���f����ǂݍ��� )
  resource = ResourceManager::Instance().LoadModelResource(filepath);

  // �m�[�h
  const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

  // �擾�����S�m�[�h������
  nodes.resize(resNodes.size());
  for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
  {
    auto&& src = resNodes.at(nodeIndex); // 3D���f���̃m�[�h���
    auto&& dst = nodes.at(nodeIndex); // �m�[�h���i�[����ׂ̃R���e�i�i���̎��_�ł͒��g�͋�j

    dst.name = src.name.c_str();
    dst.index = static_cast<int>(nodeIndex);
    dst.scale = src.scale;
    dst.rotate = src.rotate;
    dst.translate = src.translate;

    // �m�[�h���ɐe�ԍ��������Ă�����A
    // �R���e�i���炻�̔ԍ��ڂ̃A�h���X���擾��parent�ϐ��Ɋi�[ ( �e�m�[�h���i�[ )
    dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;

    if (dst.parent != nullptr) // �e�m�[�h��񂪊i�[����Ă�����	�i���g�͎q�m�[�h �m��j
    {
      // �e�m�[�h��children�R���e�i�Ɏ��g��o�^ ( �e�q�֌W���쐬 ) 
      dst.parent->children.emplace_back(&dst);
    }
  }

  // �P�ʍs��������p���Ƃ��Đݒ�
  UpdateTransform(DirectX::XMMatrixIdentity());

  // Renderer�ɓo�^
  modelRenderer->Register(this);
}

void CompModel::Update(const float& elapsedTime)
{
  gameObject.lock()->transform.UpdateTransform();

  // �A�j���[�V�����̍X�V
  animator.UpdateAnimations(elapsedTime);

  // �p���s��̍X�V
  UpdateTransform(gameObject.lock()->transform.GetWorldTransform());
}

void CompModel::OnDestroy()
{
  renderer->Remove(this);
}

void CompModel::AddCompCollisions()
{
  ObjectTransform& transform = gameObject.lock()->transform;

  // �I�u�W�F�N�g�̍s��X�V
  transform.UpdateTransform();
  UpdateTransform(transform.GetWorldTransform());

  AddCollisionsFromResource(resource.get());
}

// �p���s��v�Z
void CompModel::UpdateTransform(const DirectX::XMMATRIX& Transform)
{
  if (nodes.empty())return;

  for (Node& node : nodes)
  {
    node.oldWorldTransform = node.worldTransform;

    // ���[�J���s��Z�o
    DirectX::XMMATRIX s = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
    DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
    DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
    node.localTransform = s * r * t;

    // ���[���h�s��Z�o
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

// �m�[�h����
CompModel::Node* CompModel::FindNode(const char* name)
{
  // �S�Ẵm�[�h�𑍓�����Ŗ��O��r����
  for (int i = 0; i < nodes.size(); ++i)
  {
    // string�^���g�����@
    {
      std::string name1 = nodes.at(i).name;
      std::string name2 = name;

      if (name1 == name2) {
        return &nodes.at(i);
      }
    }

  }

  // ������Ȃ�����
  return nullptr;

}
