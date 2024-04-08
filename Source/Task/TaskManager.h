#pragma once

#include "Task.h"

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>

class TaskManager {
public:
  TaskManager() {}

  void Initialize();
  void Finalize();

  void Update(const float& elapsedTime);

  // 登録
  void Register(TASK_LAYER layer, std::shared_ptr<Task> task);
  // 削除
  void Remove(TASK_LAYER layer, std::shared_ptr<Task> task);

private:
  // マルチスレッド用関数
  void ThreadFunction(int index);

private:
  // タスクのコンテナ
  std::vector<std::shared_ptr<Task>> taskList[static_cast<int>(TASK_LAYER::Max)];

  // タスクのキュー ( スレッドプール )
  std::queue<Task*> taskQueue;

  // タスクキューの排他処理
  std::mutex queueMutex;

  // マルチスレッドで使用するフレーム間時間
  inline static float ElapsedTime = 0;

  // 使用するスレッドの数 ( メインスレッド含む )
  static const int THREAD_VOLUME = 7;

  // タスク処理に使用するスレッド
  std::unique_ptr<std::thread> subThread[THREAD_VOLUME - 1]; 
  
  Task* threadTask[THREAD_VOLUME - 1] = {};

  // スレッドプールを待機させるための物
  std::condition_variable condition;

  // スレッドの実行フラグ
  bool isRunning = true;

};
