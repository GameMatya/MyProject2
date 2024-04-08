#include "TaskManager.h"
#include "assert.h"

void TaskManager::Finalize()
{
  // �X���b�h�̎��s�t���O��False��
  isRunning = false;

  // �X���b�h�����ׂɑҋ@���̃X���b�h���N����
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
    // ���݂̃��C���[�Ɋ��蓖�Ă��Ă���^�X�N�̗ʂ��擾
    if (taskList[layer].size() == 0)continue;

    // �X���b�h�v�[���Ƀ^�X�N��ǉ�
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      for (auto& task : taskList[layer]) {
        taskQueue.push(task.get());
      }
    }

    // �ҋ@�X���b�h���N����
    condition.notify_one();

    while (true) {
      Task* task = nullptr;

      // Mutex�u���b�N
      {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Task Queue ����̎�
        if (taskQueue.empty()) {
          bool finishLayer = true;
          for (int i = 0; i < THREAD_VOLUME - 1; ++i) {
            if (threadTask[i] != nullptr)
              finishLayer = false;
          }

          // ���[�v�I��
          if (finishLayer)
            break;

          else
            continue;
        }

        // Task Queue ����, Task �����o��
        task = std::move(taskQueue.front());
        taskQueue.pop();
      }

      // Task �����s����
      task->Update(elapsedTime);
    }

    // ���̃��C���[��
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

// thread�p�̊֐�
void TaskManager::ThreadFunction(int index)
{
  // �������[�v
  while (true) {
    threadTask[index] = nullptr;

    {
      // Task Queue ����łȂ�, ��������, ���s�t���O��False�ɂȂ�܂őҋ@
      std::unique_lock<std::mutex> lock(queueMutex);
      condition.wait(lock, [&] {return !taskQueue.empty() || !isRunning; });

      // ���s�t���O��False, ����, Task Queue ����̎�
      if (!isRunning && taskQueue.empty()) {
        // �X���b�h�̏I��
        return;
      }

      // Task Queue ����, Task �����o��
      threadTask[index] = std::move(taskQueue.front());
      taskQueue.pop();
    }

    // Task �����s����
    threadTask[index]->Update(ElapsedTime);
  }
}
