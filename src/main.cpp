#include "config/Config.h"
#include "keybdHook/KeybdHook.h"
#include "mouseHook/MouseHook.h"
#include "window/Window.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
  if (AllocConsole()) {
    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
  }
#endif

  auto &config_ = Config::Instance();
  if (!config_.load()) { return -1; }

  Window win;

  MouseHook::setGlobalMouseHook(&win, &config_);
  KeybdHook::setGlobalKeybdHook(&win);

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  MouseHook::unhookGlobalMouseHook();
  KeybdHook::unhookGlobalKeybdHook();

  return 0;
}
