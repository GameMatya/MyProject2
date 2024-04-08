#pragma once

#include <vector>
#include <string>
#include "BehaviorTree.h"
#include "ActionBase.h"
#include "JudgmentBase.h"

#include <random>

// ���������[�N�����p
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// �m�[�h
template<class Owner,typename ActionEnum>
class NodeBase
{
public:
	// �R���X�g���N�^
	NodeBase(ActionEnum actionId,NodeBase* parent,NodeBase* sibling,int priority,
		SELECT_RULE selectRule,JudgmentBase<Owner>* judgment,ActionBase<Owner>* action, int hierarchyNo) :
		actionId(actionId),parent(parent),sibling(sibling),priority(priority),
		selectRule(selectRule),judgment(judgment),action(action), hierarchyNo(hierarchyNo),
		children(NULL){}

	// �f�X�g���N�^
	~NodeBase()
	{
		delete judgment;
		delete action;
	}

	// �s��Id
	ActionEnum GetActionId() { return actionId; }
	// �e�m�[�h�Q�b�^�[
	NodeBase* GetParent() { return parent; }
	// �q�m�[�h�Q�b�^�[
	NodeBase* GetChild(int index)
	{
		if (children.size() > index)
		{
			return nullptr;
		}
		return children.at(index);
	}
	// �q�m�[�h�Q�b�^�[(����)
	NodeBase* GetLastChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(children.size() - 1);
	}
	// �q�m�[�h�Q�b�^�[(�擪)
	NodeBase* GetTopChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(0);
	}
	// �Z��m�[�h�Q�b�^�[
	NodeBase* GetSibling() { return sibling; }
	// �D�揇�ʃQ�b�^�[
	int GetPriority() { return priority; }
	// �K�w�ԍ��Q�b�^�[
	int GetHirerchyNo() { return hierarchyNo; }
	// �e�m�[�h�Z�b�^�[
	void SetParent(NodeBase* parent) { this->parent = parent; }
	// �q�m�[�h�ǉ�
	void AddChild(NodeBase* child) { children.emplace_back(child); }
	// �Z��m�[�h�Z�b�^�[
	void SetSibling(NodeBase* sibling) {this->sibling = sibling;}
	// �s���f�[�^�������Ă��邩
	bool HasAction() { return action != nullptr ? true : false; }
	// ���s�۔���
	bool Judgment()
	{
		// judgment�����邩���f�B����΃����o�֐�Judgment()���s�������ʂ����^�[���B
		if (judgment != nullptr)
		{
			return judgment->Judgment();

		}
		return true;
	}

public:
	// �D�揇�ʑI��
	NodeBase* SelectPriority(std::vector<NodeBase*>* list)
	{
		NodeBase* selectNode = nullptr;
		int priority = 0;

		// ��ԗD�揇�ʂ������m�[�h��T����selectNode�Ɋi�[
		for (int i = 0; i < list->size(); i++)
		{
			if (list->at(i)->priority >=  priority)
			{
				priority = list->at(i)->priority;
				selectNode = list->at(i);
			}
		}

		return selectNode;
	}

	// �����_���I��
	NodeBase* SelectRandom(std::vector<NodeBase*>* list)
	{
		int selectNo = 0;
		selectNo = rand() % (list->size());

		return (*list).at(selectNo);
	}

	// �V�[�P���X�I��
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, SequenceData<Owner, ActionEnum>* data)
	{
		int step = 0;

		// �w�肳��Ă��钆�ԃm�[�h�̂��V�[�P���X���ǂ��܂Ŏ��s���ꂽ���擾����
		step = data->GetSequenceStep(actionId);

		// ���ԃm�[�h�ɓo�^����Ă���m�[�h���ȏ�̏ꍇ�A
		if (step >= children.size())
		{
			// ���[���ɂ���ď�����؂�ւ���
			if (selectRule == SELECT_RULE::SEQUENTIAL_LOOP) {
				step = 0;
			}
			else if (selectRule == SELECT_RULE::SEQUENCE) {
				return nullptr;
			}
		}
		// ���s�\���X�g�ɓo�^����Ă���f�[�^�̐��������[�v���s��
		for (auto itr = list->begin(); itr != list->end(); itr++)
		{
			// �q�m�[�h�����s�\���X�g�Ɋ܂܂�Ă��邩
			if (children.at(step)->GetActionId() == (*itr)->GetActionId())
			{
				data->PushSequenceNode(children.at(step)->GetParent());
				data->SetSequenceStep(actionId, step + 1);

				return children.at(step);
			}
		}
		// �w�肳�ꂽ���ԃm�[�h�Ɏ��s�\�m�[�h���Ȃ��̂�nullptr�����^�[������
		return nullptr;
	}

public:
	// �m�[�h����
	NodeBase* SearchNode(ActionEnum searchAction)
	{
		// ���O����v
		if (actionId == searchAction)
		{
			return this;
		}
		else {
			// �q�m�[�h�Ō���
			for (auto itr = children.begin(); itr != children.end(); itr++)
			{
				NodeBase* ret = (*itr)->SearchNode(searchAction);

				if (ret != nullptr)
				{
					return ret;
				}
			}
		}

		return nullptr;
	}

	// �m�[�h���_
	NodeBase* Inference(SequenceData<Owner,ActionEnum>* data)
	{
		std::vector<NodeBase*> list;
		NodeBase* result = nullptr;

		// children�̐��������[�v���s���B
		for (int i = 0; i < children.size(); i++)
		{
			// children.at(i)->judgment��nullptr�łȂ����
			if (children.at(i)->judgment != nullptr)
			{
				// children.at(i)->judgment->Judgment()�֐������s���Ature�ł����
				// list��children.at(i)��ǉ����Ă���
				if (children.at(i)->judgment->Judgment())
					list.emplace_back(children.at(i));
			}
			else {
				// ����N���X���Ȃ���Ζ������ɒǉ�
				list.emplace_back(children.at(i));
			}
		}

		// �I�����[���Ńm�[�h����
		switch (selectRule)
		{
			// �D�揇��
		case SELECT_RULE::PRIORITY:
			result = SelectPriority(&list);
			break;
			// �����_��
		case SELECT_RULE::RANDOM:
			result = SelectRandom(&list);
			break;
			// �V�[�P���X
		case SELECT_RULE::SEQUENCE:
		case SELECT_RULE::SEQUENTIAL_LOOP:
			result = SelectSequence(&list, data);
			break;
		}

		if (result != nullptr)
		{
			// �s��������ΏI��
			if (result->HasAction() == true)
			{
				return result;
			}
			else {
				// ���܂����m�[�h�Ő��_�J�n
				result = result->Inference(data);
			}
		}

		return result;
	}

	// ���s
	ActionState Run(const float& elapsedTime)
	{
		// action�����邩���f�B����΃����o�֐�Run()���s�������ʂ����^�[���B
		if (action != nullptr)
		{
			return action->Run(elapsedTime);
		}

		return ActionState::Failed;
	}

	std::vector<NodeBase*> children;		    // �q�m�[�h

protected:
	ActionEnum						actionId;			// �s��ID
	SELECT_RULE						selectRule;		// �I�����[��
	JudgmentBase<Owner>*	judgment;	    // ����N���X
	ActionBase<Owner>*		action;			  // ���s�N���X
	unsigned int					priority;		  // �D�揇��
	NodeBase*							parent;			  // �e�m�[�h
	NodeBase*							sibling;		  // �Z��m�[�h
	int										hierarchyNo;	// �K�w�ԍ�

};