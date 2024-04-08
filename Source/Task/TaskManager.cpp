#include "TaskManager.h"
#include "assert.h"

void TaskManager::Finalize()
{
  // スレッドの実行フラグをFalseに
  isRunning = false;

  // スレッドを閉じる為に待機中のスレッドを起こす
  condition.notify_all();

  for (int i = 0; i < THREAD_VOLUME - 1; ++i) {
    if (subThread[i] != nullptr)
      subThread[i]->join();
  }
}

void TaskManager::Initialize()
{
  for (int i = 0; i < THREAD_VOLUME - 1; ++i) {
    subThread[i] = std::make_unique<std::thread>(&TaskManager::ThreadFunction, this, i);
  }
}

void TaskManager::Update(const float& elapsedTime)
{
  ElapsedTime = elapsedTime;

  for (int layer = 0; layer < static_cast<int>(TASK_LAYER::Max); ++layer) {
    // 現在のレイヤーに割り当てられているタスクの量を取得
    if (taskList[layer].size() == 0)continue;

    // スレッドプールにタスクを追加
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      for (auto& task : taskList[layer]) {
        taskQueue.push(task.get());
      }
    }

    // 待機スレッドを起こす
    condition.notify_one();

    while (true) {
      Task* task = nullptr;

      // Mutexブロック
      {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Task Queue が空の時
        if (taskQueue.empty()) {
          bool finishLayer = true;
          for (int i = 0; i < THREAD_VOLUME - 1; ++i) {
            if (threadTask[i] != nullptr)
              finishLayer = false;
          }

          // ループ終了
          if (finishLayer)
            break;

          else
            continue;
        }

        // Task Queue から, Task を取り出す
        task = std::move(taskQueue.front());
        taskQueue.pop();
      }

      // Task を実行する
      task->Update(elapsedTime);
    }

    // 次のレイヤーへ
  }
}

void TaskManager::Register(TASK_LAYER layer, std::shared_ptr<Task> task)
{
  taskList[static_cast<int>(layer)].emplace_back(task);
}

void TaskManager::Remove(TASK_LAYER layer, std::shared_ptr<Task> task)
{
  std::vector<std::shared_ptr<Task>>::iterator it = std::find(taskList[static_cast<int>(layer)].begin(), taskList[static_cast<int>(layer)].end(), task);
  assert(it != taskList[static_cast<int>(layer)].end());

  taskList[static_cast<int>(layer)].erase(it);
}

// thread用の関数
void TaskManager::ThreadFunction(int index)
{
  // 無限ループ
  while (true) {
    threadTask[index] = nullptr;

    {
      // Task Queue が空でない, もしくは, 実行フラグがFalseになるまで待機
      std::unique_lock<std::mutex> lock(queueMutex);
      condition.wait(lock, [&] {return !taskQueue.empty() || !isRunning; });

      // 実行フラグがFalse, かつ, Task Queue が空の時
      if (!isRunning && taskQueue.empty()) {
        // スレッドの終了
        return;
      }

      // Task Queue から, Task を取り出す
      threadTask[index] = std::move(taskQueue.front());
      taskQueue.pop();
    }

    // Task を実行する
    threadTask[index]->Update(ElapsedTime);
  }
}
