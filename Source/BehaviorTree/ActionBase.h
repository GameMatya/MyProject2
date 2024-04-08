#pragma once

// ���s���
enum class ActionState
{
  Run,		// ���s��
  Failed,		// ���s���s
  Complete,	// ���s����
};

// �s���������N���X
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

      // ���݂̃X�e�[�g������
      if (state == ActionState::Complete)
      {
        Exit(elapsedTime);
        step = 0;
      }

      // ���݂̃X�e�[�g�̏󋵂�Ԃ�
      return state;
    }
    default:
      break;
    }

    // �G���[
    return ActionState::Failed;
  }

protected:
  // �X�e�[�g�ɓ������ۂ̏���
  virtual void Enter(const float& elapsedTime) {};

  // ���s����
  virtual ActionState Update(const float& elapsedTime) = 0;

  // �X�e�[�g�𔲂���ۂ̏���
  virtual void Exit(const float& elapsedTime) {};

  // ������
  void ResetStep() { step = 0; }

protected:
  Owner* owner;

private:
  int step = 0;

};
