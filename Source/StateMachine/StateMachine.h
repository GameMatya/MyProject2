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
  // �R�s�[�R���X�g���N�^�h�~
  StateMachine(StateMachine const&) = delete;
  StateMachine& operator=(StateMachine const&) = delete;

  ~StateMachine() {
    stateList.clear();
  }

  void AddState(StatePtr state) {
    stateList.emplace_back(state);
  }

  void Update(const float& elapsedTime) {
    if (currentState != nullptr)
    {
      oldIndex = currentIndex;
      currentState->Execute(elapsedTime);
    }
  }

  void ChangeState(Enum index) {
    // ���݂̃X�e�[�g���I��������
    if (currentState != nullptr)
    {
      currentState->Exit();
    }
    // �V�����X�e�[�g�ɐ؂�ւ��A�Ăяo��
    oldIndex = currentIndex;
    currentIndex = index;
    if (static_cast<int>(index) < 0)
    {
      currentState = nullptr;
      return;
    }

    currentState = stateList[static_cast<int>(index)];
    currentState->Enter();
  }

  Enum GetCurrentState() { return currentIndex; }
  Enum GetOldState() { return oldIndex; }

private:
  StatePtr currentState = nullptr;
  Enum currentIndex;
  Enum oldIndex;

  std::vector<StatePtr> stateList;

};
