#pragma once

#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

// Behavior保存データ
template<class Owner, typename ActionEnum>
class SequenceData
{
public:
  // コンストラクタ
  SequenceData() { Init(); }

  // 初期化
  void Init()
  {
    runSequenceStepMap.clear();
    while (sequenceStack.size() > 0)
    {
      sequenceStack.pop();
    }
  }

  // シーケンスノードのプッシュ
  void PushSequenceNode(NodeBase<Owner, ActionEnum>* node) { sequenceStack.push(node); }

  // シーケンスノードのポップ
  NodeBase<Owner, ActionEnum>* PopSequenceNode()
  {
    // 空ならNULL
    if (sequenceStack.empty() != 0)
    {
      return nullptr;
    }
    NodeBase<Owner, ActionEnum>* node = sequenceStack.top();
    if (node != nullptr)
    {
      // 取り出したデータを削除
      sequenceStack.pop();
    }
    return node;
  }

  // シーケンスステップのゲッター
  int GetSequenceStep(ActionEnum actionId)
  {
    if (runSequenceStepMap.count(actionId) == 0)
    {
      runSequenceStepMap.insert(std::make_pair(actionId, 0));
    }

    return runSequenceStepMap.at(actionId);
  }

  // シーケンスステップのセッター
  void SetSequenceStep(ActionEnum actionId, int step) { runSequenceStepMap.at(actionId) = step; }

private:
  std::stack<NodeBase<Owner, ActionEnum>*> sequenceStack;			// 実行する中間ノードをスタック
  std::map<ActionEnum, int> runSequenceStepMap;		// 実行中の中間ノードのステップを記録

};
