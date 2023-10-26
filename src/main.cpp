#include "config/Config.h"
#include "keybdHook/KeybdHook.h"
#include "mouseHook/MouseHook.h"
#include "window/Window.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   if (AllocConsole()) {
     FILE *stream;
     // stdout
     freopen_s(&stream, "CONOUT$", "w", stdout);
     // stderr
     freopen_s(&stream, "CONOUT$", "w", stderr);
     // stdin
     freopen_s(&stream, "CONIN$", "r", stdin);
   }

  auto &config_ = Config::Instance();
  config_.load();

  Window win;
  win.show();

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
