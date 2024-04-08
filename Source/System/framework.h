#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <array>

#include <d3d11.h>

#include <wrl.h>
#include <memory>

#include "misc.h"
#include "high_resolution_timer.h"
#include "Graphics/Graphics.h"
#include "System/Input.h"

#ifdef USE_IMGUI
#include "../External/imgui/imgui.h"
#include "../External/imgui/imgui_internal.h"
#include "../External/imgui/imgui_impl_dx11.h"
#include "../External/imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#ifdef _DEBUG
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
#else
CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
#endif 

static const float SCREEN_DIFFERENCE_SCALE = 1920.0f / 1280.0f;

CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"MyGame" };

class framework
{
public:
  framework(HWND hwnd);
  ~framework();

  // コピーコンストラクタ防止
  framework(const framework&) = delete;
  framework& operator=(const framework&) = delete;
  framework(framework&&) noexcept = delete;
  framework& operator=(framework&&) noexcept = delete;

  int run();

  LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
  void update(float elapsed_time);
  void render(float elapsed_time);
  void calculate_frame_stats();

private:
  CONST HWND hwnd;
  high_resolution_timer timer;
  Graphics				graphics;
  Input input;
  uint32_t frames{ 0 };
  float elapsed_time{ 0.0f };

};

