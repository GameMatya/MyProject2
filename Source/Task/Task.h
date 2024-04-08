#pragma once

// マルチタスクのレイヤー ( 同じレイヤー間では互いに干渉できない )
enum class TASK_LAYER
{
  NONE = -1, // 静的なオブジェクト
  FIRST = 0,
  SECOND,
  THIRD,
  FOUR,

  Max
};

// マルチスレッドで実行する処理
class Task
{
public:
  virtual void Update(const float& elapsedTime) = 0;
};
