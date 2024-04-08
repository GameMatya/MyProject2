#pragma once

#include <deque>

using GamePadButton = unsigned int;

// ゲームパッド
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
    BTN_LEFT_THUMB     = (1 << 10), // スティック押し込み
    BTN_RIGHT_THUMB    = (1 << 11), // スティック押し込み
    BTN_LEFT_SHOULDER  = (1 << 12), // L1
    BTN_RIGHT_SHOULDER = (1 << 13), // R1
    BTN_LEFT_TRIGGER   = (1 << 14), // L2
    BTN_RIGHT_TRIGGER  = (1 << 15)  // R2
  };

public:
  GamePad() { buttonQueue.resize(QUEUE_SIZE); }
  ~GamePad() {}

  // 更新
  void Update();

  // スロット設定
  void SetSlot(int slot) { this->slot = slot; }

  // ボタン入力状態の取得
  bool GetButton() const { return buttonState[0] > 0; }
  bool GetButton(const GamePadButton& button) const;
  // ボタン押下状態の取得
  bool GetButtonDown(const GamePadButton& button, const int& frame = 0) const;
  // ボタン押上状態の取得
  bool GetButtonUp(const GamePadButton& button) const;

  // 左スティックX軸入力状態の取得
  float GetAxisLX() const { return axisLx; }
  // 左スティックY軸入力状態の取得
  float GetAxisLY() const { return axisLy; }
  // 左スティック入力状態の取得
  bool GetStickL() const { return axisLy + axisLx; }

  // 右スティックX軸入力状態の取得
  float GetAxisRX() const { return axisRx; }
  // 右スティックY軸入力状態の取得
  float GetAxisRY() const { return axisRy; }
  // 右スティック入力状態の取得
  bool GetStickR() const { return axisRy + axisRx; }

  // 左トリガー入力状態の取得
  float GetTriggerL() const { return triggerL; }
  // 右トリガー入力状態の取得
  float GetTriggerR() const { return triggerR; }

  // コントローラーの接続確認
  bool IsConnect() const { return isConnect; }

private:
  // 過去の入力データを格納する
  std::deque<GamePadButton> buttonQueue;
  // 格納するデータ量
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
