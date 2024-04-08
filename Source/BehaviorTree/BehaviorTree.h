#pragma once
#include <string>

template<class Owner, typename ActionEnum> class NodeBase;
template<class Owner, typename ActionEnum> class SequenceData;
template<class Owner> class JudgmentBase;
template<class Owner> class ActionBase;

// �I�����[��
enum class SELECT_RULE
{
  NONE,							// �������[�m�[�h�p
  PRIORITY,					// �D�揇��
  SEQUENCE,					// ���Ԃɏ���
  SEQUENTIAL_LOOP,	// �V�[�P���V�������[�s���O
  RANDOM,						// �����_��
};

// �r�w�C�r�A�c���[
template<class Owner, typename ActionEnum>
class BehaviorTree
{
public:
  BehaviorTree(Owner* owner);

  ~BehaviorTree() {
    NodeAllClear(root);
    delete sequenceData;
  }

  void Update(const float& elapsedTime)
  {
    // ���ݎ��s����Ă���m�[�h���������
    if (activeNode == nullptr)
    {
      // ���Ɏ��s����m�[�h�𐄘_����B
      activeNode = ActiveNodeInference(sequenceData);
    }

    // ���ݎ��s����m�[�h�������
    if (activeNode != nullptr)
    {
      // �r�w�C�r�A�c���[����m�[�h�����s�B
      activeNode = Run(activeNode, sequenceData, elapsedTime);
    }
  }

  // �m�[�h�ǉ�
  void AddNode(const ActionEnum& parent, const ActionEnum& entry, const int& priority, const SELECT_RULE& selectRule, JudgmentBase<Owner>* judgment, ActionBase<Owner>* action)
  {
    NodeBase<Owner, ActionEnum>* parentNode = root->SearchNode(parent);

    if (parentNode != nullptr)
    {
      NodeBase<Owner, ActionEnum>* sibling = parentNode->GetLastChild();
      NodeBase<Owner, ActionEnum>* addNode = new NodeBase<Owner, ActionEnum>(entry, parentNode, sibling, priority, selectRule, judgment, action, parentNode->GetHirerchyNo() + 1);

      parentNode->AddChild(addNode);
    }
    else
    {
      assert(1);
    }
  }

  NodeBase<Owner, ActionEnum>* GetActiveNode() { return activeNode; }

  const int GetOldAction() {
    return oldAction;
  }

private:
  // ���s�m�[�h�𐄘_����
  NodeBase<Owner, ActionEnum>* ActiveNodeInference(SequenceData<Owner, ActionEnum>* data)
  {
    // �f�[�^�����Z�b�g���ĊJ�n
    data->Init();
    return root->Inference(data);
  }

  // �V�[�P���X�m�[�h���琄�_�J�n
  NodeBase<Owner, ActionEnum>* SequenceBack(NodeBase<Owner, ActionEnum>* sequenceNode, SequenceData<Owner, ActionEnum>* data)
  {
    return sequenceNode->Inference(data);
  }

  // ���s
  NodeBase<Owner, ActionEnum>* Run(NodeBase<Owner, ActionEnum>* actionNode, SequenceData<Owner, ActionEnum>* data, const float& elapsedTime);

  // �m�[�h�S�폜
  void NodeAllClear(NodeBase<Owner, ActionEnum>* delNode)
  {
    size_t count = delNode->children.size();
    if (count > 0)
    {
      for (NodeBase<Owner, ActionEnum>* node : delNode->children)
      {
        NodeAllClear(node);
      }
      delete delNode;
    }
    else
    {
      delete delNode;
    }
  }

private:
  Owner* owner = nullptr;
  // ���[�g�m�[�h
  NodeBase<Owner, ActionEnum>* root = nullptr;
  // �V�[�N�G���X
  SequenceData<Owner, ActionEnum>* sequenceData = nullptr;
  // ���s����m�[�h�̃|�C���^
  NodeBase<Owner, ActionEnum>* activeNode = nullptr;
  // �O�Ɏ��s���Ă����s�� ( �r�b�g )
  int oldAction = 0;
  int actionCache = 0;

};

#include "NodeBase.h"
#include "SequenceData.h"

template<class Owner, typename ActionEnum>
inline BehaviorTree<Owner, ActionEnum>::BehaviorTree(Owner* owner) :root(nullptr), owner(owner)
{
  sequenceData = new SequenceData<Owner, ActionEnum>();
  root = new NodeBase<Owner, ActionEnum>(static_cast<ActionEnum>(0), nullptr, nullptr, 0, SELECT_RULE::PRIORITY, nullptr, nullptr, 1);
}

template<class Owner, typename ActionEnum>
inline NodeBase<Owner, ActionEnum>* BehaviorTree<Owner, ActionEnum>::Run(NodeBase<Owner, ActionEnum>* actionNode, SequenceData<Owner, ActionEnum>* data, const float& elapsedTime)
{
  // �m�[�h���s
  ActionState state = actionNode->Run(elapsedTime);

  // ����I��
  if (state == ActionState::Complete)
  {
    // �V�[�P���X�̓r�����𔻒f
    NodeBase<Owner, ActionEnum>* sequenceNode = data->PopSequenceNode();

    actionCache |= (1 << static_cast<int>(actionNode->GetActionId()));

    // �r������Ȃ��Ȃ�I��
    if (sequenceNode == nullptr)
    {
      oldAction = 0;
      oldAction = actionCache;
      actionCache = 0;

      return nullptr;
    }
    else {
      oldAction = actionCache;

      // �r���Ȃ炻������n�߂�
      return SequenceBack(sequenceNode, data);
    }
    // ���s�͏I��
  }
  else if (state == ActionState::Failed) {
    return nullptr;
  }

  // ����ێ�
  return actionNode;
}
