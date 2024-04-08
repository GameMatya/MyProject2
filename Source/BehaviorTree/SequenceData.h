#pragma once

#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

// Behavior�ۑ��f�[�^
template<class Owner, typename ActionEnum>
class SequenceData
{
public:
  // �R���X�g���N�^
  SequenceData() { Init(); }

  // ������
  void Init()
  {
    runSequenceStepMap.clear();
    while (sequenceStack.size() > 0)
    {
      sequenceStack.pop();
    }
  }

  // �V�[�P���X�m�[�h�̃v�b�V��
  void PushSequenceNode(NodeBase<Owner, ActionEnum>* node) { sequenceStack.push(node); }

  // �V�[�P���X�m�[�h�̃|�b�v
  NodeBase<Owner, ActionEnum>* PopSequenceNode()
  {
    // ��Ȃ�NULL
    if (sequenceStack.empty() != 0)
    {
      return nullptr;
    }
    NodeBase<Owner, ActionEnum>* node = sequenceStack.top();
    if (node != nullptr)
    {
      // ���o�����f�[�^���폜
      sequenceStack.pop();
    }
    return node;
  }

  // �V�[�P���X�X�e�b�v�̃Q�b�^�[
  int GetSequenceStep(ActionEnum actionId)
  {
    if (runSequenceStepMap.count(actionId) == 0)
    {
      runSequenceStepMap.insert(std::make_pair(actionId, 0));
    }

    return runSequenceStepMap.at(actionId);
  }

  // �V�[�P���X�X�e�b�v�̃Z�b�^�[
  void SetSequenceStep(ActionEnum actionId, int step) { runSequenceStepMap.at(actionId) = step; }

private:
  std::stack<NodeBase<Owner, ActionEnum>*> sequenceStack;			// ���s���钆�ԃm�[�h���X�^�b�N
  std::map<ActionEnum, int> runSequenceStepMap;		// ���s���̒��ԃm�[�h�̃X�e�b�v���L�^

};
