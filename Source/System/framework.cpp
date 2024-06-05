#include "framework.h"
#include "SceneSystem/SceneManager.h"
#include "Scenes/SceneTitle.h"
#include "SceneSystem/SceneLoading.h"
#include "Audio/Audio.h"
#include "Graphics/EffectManager.h"
#include "TimeManager/TimeManager.h"

static const UINT SYNC_INTERVAL{ 1 }; // 1 にするとFPSの上限が60Fになる

framework::framework(HWND hwnd) :
  hwnd(hwnd)
  , graphics(hwnd)
  , input(hwnd)
{
  // オーディオ初期化
  Audio::Initalize();

  // エフェクシア初期化
  EffectManager::Instance().Initialize();

  // シーン初期化
  SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
}

framework::~framework()
{
  // シーン終了化
  SceneManager::Instance().Clear();
}

void framework::update(float elapsedTime/*Elapsed seconds from last frame*/)
{
  // ImGuiのフレームを生成
#ifdef USE_IMGUI
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  //ImGuiドッキングスペース
  //ImGuiDockNodeFlags_PassthruCentralNodeは
  //ドッキングスペースの背景を描画できるようにする
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
#endif
  // 入力情報更新
  input.Update();

  // 時間エフェクトの更新処理
  TimeManager& timeManager = TimeManager::Instance();
  timeManager.Update(elapsedTime);

  const float& managedTime = timeManager.GetElapsedTime(elapsedTime);

  SceneManager::Instance().Update(managedTime);

  // エフェクシアの描画
  EffectManager::Instance().Update(managedTime);
}

void framework::render(float elapsedTime/*Elapsed seconds from last frame*/)
{
  std::unique_lock<std::mutex> lock(graphics.GetMutex());

  // 各シーンの描画
  SceneManager::Instance().Render();

  // ImGuiの描画
#ifdef USE_IMGUI
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

#endif

  // バックバッファに描画した画を画面に表示する。
  graphics.GetSwapChain()->Present(SYNC_INTERVAL, 0);

}

int framework::run()
{
  MSG msg{};

#ifdef USE_IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(graphics.GetDevice(), graphics.GetDeviceContext());
  ImGui::StyleColorsDark();
#endif

  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      timer.tick();
      calculate_frame_stats();

      update(timer.time_interval());
      render(timer.time_interval());
    }
  }

#ifdef USE_IMGUI
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
#endif

#if 1
  BOOL fullscreen = 0;
  graphics.GetSwapChain()->GetFullscreenState(&fullscreen, 0);
  if (fullscreen)
  {
    graphics.GetSwapChain()->SetFullscreenState(FALSE, 0);
  }
#endif

  return static_cast<int>(msg.wParam);
}

LRESULT framework::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
  switch (msg)
  {
  case WM_PAINT:
  {
    PAINTSTRUCT ps{};
    BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    break;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_CREATE:
    break;
  case WM_KEYDOWN:
    if (wparam == VK_ESCAPE)
    {
      PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    break;
  case WM_ENTERSIZEMOVE:
    timer.stop();
    break;
  case WM_EXITSIZEMOVE:
    timer.start();
    break;
  case WM_MOUSEWHEEL:
    Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wparam));
    break;
  default:
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  return 0;
}

void framework::calculate_frame_stats()
{
  // Code computes the average frames per second, and also the 
  // average time it takes to render one frame.  These stats 
  // are appended to the window caption bar.
  static int frames = 0;
  static float time_tlapsed = 0.0f;

  frames++;

  // Compute averages over one second period.
  if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
  {
    float fps = static_cast<float>(frames); // fps = frameCnt / 1
    float mspf = 1000.0f / fps;
    std::ostringstream outs;
    outs.precision(6);
    outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
    SetWindowTextA(hwnd, outs.str().c_str());

    // Reset for next average.
    frames = 0;
    time_tlapsed += 1.0f;
  }

}