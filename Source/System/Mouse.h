#pragma once

#include <Windows.h>
#include <deque>

using MouseButton = unsigned int;

// �}�E�X
class Mouse
{
public:
  static const MouseButton BTN_LEFT = (1 << 0);
  static const MouseButton BTN_MIDDLE = (1 << 1);
  static const MouseButton BTN_RIGHT = (1 << 2);

public:
  Mouse(HWND hWnd);
  ~Mouse() {}

  // �X�V
  void Update();

  // �{�^�����͏�Ԃ̎擾
  bool GetButton(const MouseButton& button) const { return buttonState & button; }
  // �{�^��������Ԃ̎擾
  bool GetButtonDown(const MouseButton& button, const int& frame = 0) const;
  // �{�^�������Ԃ̎擾
  bool GetButtonUp(const MouseButton& button) const { return buttonUp & button; }

  // �z�C�[���l�̐ݒ�
  void SetWheel(int wheel) { this->wheel[0] += wheel; }
  // �z�C�[���l�̎擾
  int GetWheel() const { return wheel[1]; }

  // �}�E�X�J�[�\��X���W�擾
  int GetPositionX() const { return positionX[0]; }
  // �}�E�X�J�[�\��Y���W�擾
  int GetPositionY() const { return positionY[0]; }

  // �O��̃}�E�X�J�[�\��X���W�擾
  int GetOldPositionX() const { return positionX[1]; }
  // �O��̃}�E�X�J�[�\��Y���W�擾
  int GetOldPositionY() const { return positionY[1]; }

  // �X�N���[�����ݒ�
  void SetScreenWidth(int width) { screenWidth = width; }
  // �X�N���[�������ݒ�
  void SetScreenHeight(int height) { screenHeight = height; }

  // �X�N���[�����擾
  int GetScreenWidth() const { return screenWidth; }
  // �X�N���[�������擾
  int GetScreenHeight() const { return screenHeight; }

private:
  // �ߋ��̓��̓f�[�^���i�[����
  std::deque<MouseButton> buttonQueue = { 0 };
  // �i�[����f�[�^��
  const int QUEUE_SIZE = 256;

  MouseButton		buttonState = {};
  MouseButton		buttonDown = 0;
  MouseButton		buttonUp = 0;
  int				positionX[2];
  int				positionY[2];
  int				wheel[2];
  int				screenWidth = 0;
  int				screenHeight = 0;
  HWND			hWnd = nullptr;
};
