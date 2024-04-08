#pragma once

#include <Windows.h>
#include <deque>

using MouseButton = unsigned int;

// マウス
class Mouse
{
public:
  static const MouseButton BTN_LEFT = (1 << 0);
  static const MouseButton BTN_MIDDLE = (1 << 1);
  static const MouseButton BTN_RIGHT = (1 << 2);

public:
  Mouse(HWND hWnd);
  ~Mouse() {}

  // 更新
  void Update();

  // ボタン入力状態の取得
  bool GetButton(const MouseButton& button) const { return buttonState & button; }
  // ボタン押下状態の取得
  bool GetButtonDown(const MouseButton& button, const int& frame = 0) const;
  // ボタン押上状態の取得
  bool GetButtonUp(const MouseButton& button) const { return buttonUp & button; }

  // ホイール値の設定
  void SetWheel(int wheel) { this->wheel[0] += wheel; }
  // ホイール値の取得
  int GetWheel() const { return wheel[1]; }

  // マウスカーソルX座標取得
  int GetPositionX() const { return positionX[0]; }
  // マウスカーソルY座標取得
  int GetPositionY() const { return positionY[0]; }

  // 前回のマウスカーソルX座標取得
  int GetOldPositionX() const { return positionX[1]; }
  // 前回のマウスカーソルY座標取得
  int GetOldPositionY() const { return positionY[1]; }

  // スクリーン幅設定
  void SetScreenWidth(int width) { screenWidth = width; }
  // スクリーン高さ設定
  void SetScreenHeight(int height) { screenHeight = height; }

  // スクリーン幅取得
  int GetScreenWidth() const { return screenWidth; }
  // スクリーン高さ取得
  int GetScreenHeight() const { return screenHeight; }

private:
  // 過去の入力データを格納する
  std::deque<MouseButton> buttonQueue = { 0 };
  // 格納するデータ量
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
