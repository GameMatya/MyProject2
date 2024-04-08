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

  // �o�^
  void Register(TASK_LAYER layer, std::shared_ptr<Task> task);
  // �폜
  void Remove(TASK_LAYER layer, std::shared_ptr<Task> task);

private:
  // �}���`�X���b�h�p�֐�
  void ThreadFunction(int index);

private:
  // �^�X�N�̃R���e�i
  std::vector<std::shared_ptr<Task>> taskList[static_cast<int>(TASK_LAYER::Max)];

  // �^�X�N�̃L���[ ( �X���b�h�v�[�� )
  std::queue<Task*> taskQueue;

  // �^�X�N�L���[�̔r������
  std::mutex queueMutex;

  // �}���`�X���b�h�Ŏg�p����t���[���Ԏ���
  inline static float ElapsedTime = 0;

  // �g�p����X���b�h�̐� ( ���C���X���b�h�܂� )
  static const int THREAD_VOLUME = 7;

  // �^�X�N�����Ɏg�p����X���b�h
  std::unique_ptr<std::thread> subThread[THREAD_VOLUME - 1]; 
  
  Task* threadTask[THREAD_VOLUME - 1] = {};

  // �X���b�h�v�[����ҋ@�����邽�߂̕�
  std::condition_variable condition;

  // �X���b�h�̎��s�t���O
  bool isRunning = true;

};
