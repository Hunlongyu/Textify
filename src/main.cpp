#include "config/Config.h"
#include "keybdHook/KeybdHook.h"
#include "mouseHook/MouseHook.h"
#include "window/Window.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  Config::Instance().load();

  Window win;

  MouseHook::setGlobalMouseHook(&win);
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
