#pragma once

// 実行情報
enum class ActionState
{
  Run,		// 実行中
  Failed,		// 実行失敗
  Complete,	// 実行成功
};

// 行動処理基底クラス
template<class Owner>
class ActionBase
{
public:
  ActionBase(Owner* owner) :owner(owner) {}

  ActionState Run(const float& elapsedTime) {
    switch (step)
    {
    case 0:
      Enter(elapsedTime);

      step++;
    case 1:
    {
      ActionState state = Update(elapsedTime);

      // 現在のステートが完了
      if (state == ActionState::Complete)
      {
        Exit(elapsedTime);
        step = 0;
      }

      // 現在のステートの状況を返す
      return state;
    }
    default:
      break;
    }

    // エラー
    return ActionState::Failed;
  }

protected:
  // ステートに入った際の処理
  virtual void Enter(const float& elapsedTime) {};

  // 実行処理
  virtual ActionState Update(const float& elapsedTime) = 0;

  // ステートを抜ける際の処理
  virtual void Exit(const float& elapsedTime) {};

  // 初期化
  void ResetStep() { step = 0; }

protected:
  Owner* owner;

private:
  int step = 0;

};
