#pragma once
#include <string>

template<class Owner, typename ActionEnum> class NodeBase;
template<class Owner, typename ActionEnum> class SequenceData;
template<class Owner> class JudgmentBase;
template<class Owner> class ActionBase;

// 選択ルール
enum class SELECT_RULE
{
  NONE,							// 無い末端ノード用
  PRIORITY,					// 優先順位
  SEQUENCE,					// 順番に処理
  SEQUENTIAL_LOOP,	// シーケンシャルルーピング
  RANDOM,						// ランダム
};

// ビヘイビアツリー
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
    // 現在実行されているノードが無ければ
    if (activeNode == nullptr)
    {
      // 次に実行するノードを推論する。
      activeNode = ActiveNodeInference(sequenceData);
    }

    // 現在実行するノードがあれば
    if (activeNode != nullptr)
    {
      // ビヘイビアツリーからノードを実行。
      activeNode = Run(activeNode, sequenceData, elapsedTime);
    }
  }

  // ノード追加
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
  // 実行ノードを推論する
  NodeBase<Owner, ActionEnum>* ActiveNodeInference(SequenceData<Owner, ActionEnum>* data)
  {
    // データをリセットして開始
    data->Init();
    return root->Inference(data);
  }

  // シーケンスノードから推論開始
  NodeBase<Owner, ActionEnum>* SequenceBack(NodeBase<Owner, ActionEnum>* sequenceNode, SequenceData<Owner, ActionEnum>* data)
  {
    return sequenceNode->Inference(data);
  }

  // 実行
  NodeBase<Owner, ActionEnum>* Run(NodeBase<Owner, ActionEnum>* actionNode, SequenceData<Owner, ActionEnum>* data, const float& elapsedTime);

  // ノード全削除
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
  // ルートノード
  NodeBase<Owner, ActionEnum>* root = nullptr;
  // シークエンス
  SequenceData<Owner, ActionEnum>* sequenceData = nullptr;
  // 実行するノードのポインタ
  NodeBase<Owner, ActionEnum>* activeNode = nullptr;
  // 前に実行していた行動 ( ビット )
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
  // ノード実行
  ActionState state = actionNode->Run(elapsedTime);

  // 正常終了
  if (state == ActionState::Complete)
  {
    // シーケンスの途中かを判断
    NodeBase<Owner, ActionEnum>* sequenceNode = data->PopSequenceNode();

    actionCache |= (1 << static_cast<int>(actionNode->GetActionId()));

    // 途中じゃないなら終了
    if (sequenceNode == nullptr)
    {
      oldAction = 0;
      oldAction = actionCache;
      actionCache = 0;

      return nullptr;
    }
    else {
      oldAction = actionCache;

      // 途中ならそこから始める
      return SequenceBack(sequenceNode, data);
    }
    // 失敗は終了
  }
  else if (state == ActionState::Failed) {
    return nullptr;
  }

  // 現状維持
  return actionNode;
}
