#pragma once

#include <vector>
#include <string>
#include "BehaviorTree.h"
#include "ActionBase.h"
#include "JudgmentBase.h"

#include <random>

// メモリリーク調査用
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// ノード
template<class Owner,typename ActionEnum>
class NodeBase
{
public:
	// コンストラクタ
	NodeBase(ActionEnum actionId,NodeBase* parent,NodeBase* sibling,int priority,
		SELECT_RULE selectRule,JudgmentBase<Owner>* judgment,ActionBase<Owner>* action, int hierarchyNo) :
		actionId(actionId),parent(parent),sibling(sibling),priority(priority),
		selectRule(selectRule),judgment(judgment),action(action), hierarchyNo(hierarchyNo),
		children(NULL){}

	// デストラクタ
	~NodeBase()
	{
		delete judgment;
		delete action;
	}

	// 行動Id
	ActionEnum GetActionId() { return actionId; }
	// 親ノードゲッター
	NodeBase* GetParent() { return parent; }
	// 子ノードゲッター
	NodeBase* GetChild(int index)
	{
		if (children.size() > index)
		{
			return nullptr;
		}
		return children.at(index);
	}
	// 子ノードゲッター(末尾)
	NodeBase* GetLastChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(children.size() - 1);
	}
	// 子ノードゲッター(先頭)
	NodeBase* GetTopChild()
	{
		if (children.size() == 0)
		{
			return nullptr;
		}

		return children.at(0);
	}
	// 兄弟ノードゲッター
	NodeBase* GetSibling() { return sibling; }
	// 優先順位ゲッター
	int GetPriority() { return priority; }
	// 階層番号ゲッター
	int GetHirerchyNo() { return hierarchyNo; }
	// 親ノードセッター
	void SetParent(NodeBase* parent) { this->parent = parent; }
	// 子ノード追加
	void AddChild(NodeBase* child) { children.emplace_back(child); }
	// 兄弟ノードセッター
	void SetSibling(NodeBase* sibling) {this->sibling = sibling;}
	// 行動データを持っているか
	bool HasAction() { return action != nullptr ? true : false; }
	// 実行可否判定
	bool Judgment()
	{
		// judgmentがあるか判断。あればメンバ関数Judgment()実行した結果をリターン。
		if (judgment != nullptr)
		{
			return judgment->Judgment();

		}
		return true;
	}

public:
	// 優先順位選択
	NodeBase* SelectPriority(std::vector<NodeBase*>* list)
	{
		NodeBase* selectNode = nullptr;
		int priority = 0;

		// 一番優先順位が高いノードを探してselectNodeに格納
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

	// ランダム選択
	NodeBase* SelectRandom(std::vector<NodeBase*>* list)
	{
		int selectNo = 0;
		selectNo = rand() % (list->size());

		return (*list).at(selectNo);
	}

	// シーケンス選択
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, SequenceData<Owner, ActionEnum>* data)
	{
		int step = 0;

		// 指定されている中間ノードのがシーケンスがどこまで実行されたか取得する
		step = data->GetSequenceStep(actionId);

		// 中間ノードに登録されているノード数以上の場合、
		if (step >= children.size())
		{
			// ルールによって処理を切り替える
			if (selectRule == SELECT_RULE::SEQUENTIAL_LOOP) {
				step = 0;
			}
			else if (selectRule == SELECT_RULE::SEQUENCE) {
				return nullptr;
			}
		}
		// 実行可能リストに登録されているデータの数だけループを行う
		for (auto itr = list->begin(); itr != list->end(); itr++)
		{
			// 子ノードが実行可能リストに含まれているか
			if (children.at(step)->GetActionId() == (*itr)->GetActionId())
			{
				data->PushSequenceNode(children.at(step)->GetParent());
				data->SetSequenceStep(actionId, step + 1);

				return children.at(step);
			}
		}
		// 指定された中間ノードに実行可能ノードがないのでnullptrをリターンする
		return nullptr;
	}

public:
	// ノード検索
	NodeBase* SearchNode(ActionEnum searchAction)
	{
		// 名前が一致
		if (actionId == searchAction)
		{
			return this;
		}
		else {
			// 子ノードで検索
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

	// ノード推論
	NodeBase* Inference(SequenceData<Owner,ActionEnum>* data)
	{
		std::vector<NodeBase*> list;
		NodeBase* result = nullptr;

		// childrenの数だけループを行う。
		for (int i = 0; i < children.size(); i++)
		{
			// children.at(i)->judgmentがnullptrでなければ
			if (children.at(i)->judgment != nullptr)
			{
				// children.at(i)->judgment->Judgment()関数を実行し、tureであれば
				// listにchildren.at(i)を追加していく
				if (children.at(i)->judgment->Judgment())
					list.emplace_back(children.at(i));
			}
			else {
				// 判定クラスがなければ無条件に追加
				list.emplace_back(children.at(i));
			}
		}

		// 選択ルールでノード決め
		switch (selectRule)
		{
			// 優先順位
		case SELECT_RULE::PRIORITY:
			result = SelectPriority(&list);
			break;
			// ランダム
		case SELECT_RULE::RANDOM:
			result = SelectRandom(&list);
			break;
			// シーケンス
		case SELECT_RULE::SEQUENCE:
		case SELECT_RULE::SEQUENTIAL_LOOP:
			result = SelectSequence(&list, data);
			break;
		}

		if (result != nullptr)
		{
			// 行動があれば終了
			if (result->HasAction() == true)
			{
				return result;
			}
			else {
				// 決まったノードで推論開始
				result = result->Inference(data);
			}
		}

		return result;
	}

	// 実行
	ActionState Run(const float& elapsedTime)
	{
		// actionがあるか判断。あればメンバ関数Run()実行した結果をリターン。
		if (action != nullptr)
		{
			return action->Run(elapsedTime);
		}

		return ActionState::Failed;
	}

	std::vector<NodeBase*> children;		    // 子ノード

protected:
	ActionEnum						actionId;			// 行動ID
	SELECT_RULE						selectRule;		// 選択ルール
	JudgmentBase<Owner>*	judgment;	    // 判定クラス
	ActionBase<Owner>*		action;			  // 実行クラス
	unsigned int					priority;		  // 優先順位
	NodeBase*							parent;			  // 親ノード
	NodeBase*							sibling;		  // 兄弟ノード
	int										hierarchyNo;	// 階層番号

};