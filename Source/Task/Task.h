#pragma once

// �}���`�^�X�N�̃��C���[ ( �������C���[�Ԃł݂͌��Ɋ��ł��Ȃ� )
enum class TASK_LAYER
{
  NONE = -1, // �ÓI�ȃI�u�W�F�N�g
  FIRST = 0,
  SECOND,
  THIRD,
  FOUR,

  Max
};

// �}���`�X���b�h�Ŏ��s���鏈��
class Task
{
public:
  virtual void Update(const float& elapsedTime) = 0;
};
