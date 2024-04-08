#pragma once

#include <deque>

using GamePadButton = unsigned int;

// �Q�[���p�b�h
class GamePad
{
public:
  enum GAMEPAD_BUTTON
  {
    BTN_UP             = (1 << 0),
    BTN_RIGHT          = (1 << 1),
    BTN_DOWN           = (1 << 2),
    BTN_LEFT           = (1 << 3),

    BTN_A              = (1 << 4),
    BTN_B              = (1 << 5),
    BTN_X              = (1 << 6),
    BTN_Y              = (1 << 7),

    BTN_START          = (1 << 8),
    BTN_BACK           = (1 << 9),
    BTN_LEFT_THUMB     = (1 << 10), // �X�e�B�b�N��������
    BTN_RIGHT_THUMB    = (1 << 11), // �X�e�B�b�N��������
    BTN_LEFT_SHOULDER  = (1 << 12), // L1
    BTN_RIGHT_SHOULDER = (1 << 13), // R1
    BTN_LEFT_TRIGGER   = (1 << 14), // L2
    BTN_RIGHT_TRIGGER  = (1 << 15)  // R2
  };

public:
  GamePad() { buttonQueue.resize(QUEUE_SIZE); }
  ~GamePad() {}

  // �X�V
  void Update();

  // �X���b�g�ݒ�
  void SetSlot(int slot) { this->slot = slot; }

  // �{�^�����͏�Ԃ̎擾
  bool GetButton() const { return buttonState[0] > 0; }
  bool GetButton(const GamePadButton& button) const;
  // �{�^��������Ԃ̎擾
  bool GetButtonDown(const GamePadButton& button, const int& frame = 0) const;
  // �{�^�������Ԃ̎擾
  bool GetButtonUp(const GamePadButton& button) const;

  // ���X�e�B�b�NX�����͏�Ԃ̎擾
  float GetAxisLX() const { return axisLx; }
  // ���X�e�B�b�NY�����͏�Ԃ̎擾
  float GetAxisLY() const { return axisLy; }
  // ���X�e�B�b�N���͏�Ԃ̎擾
  bool GetStickL() const { return axisLy + axisLx; }

  // �E�X�e�B�b�NX�����͏�Ԃ̎擾
  float GetAxisRX() const { return axisRx; }
  // �E�X�e�B�b�NY�����͏�Ԃ̎擾
  float GetAxisRY() const { return axisRy; }
  // �E�X�e�B�b�N���͏�Ԃ̎擾
  bool GetStickR() const { return axisRy + axisRx; }

  // ���g���K�[���͏�Ԃ̎擾
  float GetTriggerL() const { return triggerL; }
  // �E�g���K�[���͏�Ԃ̎擾
  float GetTriggerR() const { return triggerR; }

  // �R���g���[���[�̐ڑ��m�F
  bool IsConnect() const { return isConnect; }

private:
  // �ߋ��̓��̓f�[�^���i�[����
  std::deque<GamePadButton> buttonQueue;
  // �i�[����f�[�^��
  const int QUEUE_SIZE = 256;

  GamePadButton		buttonState[2] = {};
  GamePadButton		buttonDown = 0;
  GamePadButton		buttonUp = 0;
  float				axisLx = 0.0f;
  float				axisLy = 0.0f;
  float				axisRx = 0.0f;
  float				axisRy = 0.0f;
  float				triggerL = 0.0f;
  float				triggerR = 0.0f;
  int					slot = 0;
  bool				isConnect = false;

};
