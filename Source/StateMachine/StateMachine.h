#pragma once
#include "State.h"
#include <vector>
#include <memory>

template <class Owner, typename Enum>
class StateMachine
{
  using StatePtr = std::shared_ptr<State<Owner>>;

public:
  StateMachine() = default;
  // コピーコンストラクタ防止
  StateMachine(StateMachine const&) = delete;
  StateMachine& operator=(StateMachine const&) = delete;

  ~StateMachine() {
    stateList.clear();
  }

  void AddState(Enum index, StatePtr state) {
    stateList.emplace(index, state);
  }

  void Update(const float& elapsedTime) {
    if (currentState != nullptr)
    {
      oldIndex = currentIndex;
      currentState->Execute(elapsedTime);
    }
  }

  void ChangeState(Enum index) {
    // 現在のステートを終了させる
    if (currentState != nullptr)
    {
      nextIndex = index;
      currentState->Exit();
    }
    // 新しいステートに切り替え、呼び出し
    oldIndex = currentIndex;
    currentIndex = index;
    if (static_cast<int>(index) < 0)
    {
      currentState = nullptr;
      return;
    }

    currentState = stateList[index];
    currentState->Enter();
  }

  Enum GetCurrentState() { return currentIndex; }
  Enum GetNextState() { return nextIndex; }
  Enum GetOldState() { return oldIndex; }

private:
  StatePtr currentState = nullptr;
  Enum nextIndex;
  Enum currentIndex;
  Enum oldIndex;

  std::map<Enum, StatePtr> stateList;

};
