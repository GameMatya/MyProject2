#include "System/Mouse.h"

static const int KeyMap[] =
{
  VK_LBUTTON,		// 左ボタン
  VK_MBUTTON,		// 中ボタン
  VK_RBUTTON,		// 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd)
  : hWnd(hWnd)
{
  RECT rc;
  GetClientRect(hWnd, &rc);
  screenWidth = rc.right - rc.left;
  screenHeight = rc.bottom - rc.top;
}

// 更新
void Mouse::Update()
{
  // スイッチ情報
  MouseButton newButtonState = 0;

  for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
  {
    if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
    {
      newButtonState |= (1 << i);
    }
  }

  // ホイール
  wheel[1] = wheel[0];
  wheel[0] = 0;

  // ボタン情報更新
  {
    buttonState = newButtonState;

    buttonDown = ~buttonQueue.back() & newButtonState;	// 押した瞬間
    buttonUp = ~newButtonState & buttonQueue.back();	// 離した瞬間

    // 格納
    buttonQueue.emplace_back(buttonState);

    // サイズがオーバーしていたら先頭要素( 一番古い )を削除
    if (buttonQueue.size() >= QUEUE_SIZE) {
      buttonQueue.pop_front();
    }
  }

  // カーソル位置の取得
  {
    POINT cursor;
    ::GetCursorPos(&cursor);
    ::ScreenToClient(hWnd, &cursor);

    // 画面のサイズを取得する。
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT screenW = rc.right - rc.left;
    UINT screenH = rc.bottom - rc.top;
    UINT viewportW = screenWidth;
    UINT viewportH = screenHeight;

    // 画面補正
    positionX[1] = positionX[0];
    positionY[1] = positionY[0];
    positionX[0] = (LONG)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
    positionY[0] = (LONG)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));
  }
}

bool Mouse::GetButtonDown(const MouseButton& button, const int& frame) const
{
  if (frame > 0)
  {
    // frame分、過去の入力をチェックする
    for (int i = (int)buttonQueue.size() - 1; i > (int)buttonQueue.size() - frame; --i) {
      if (buttonQueue.at(i) & button)
        return true;
    }
    return false;
  }
  else
    return (buttonDown & button);
}
