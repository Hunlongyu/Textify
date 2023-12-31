﻿#include "config/CWin.h"
#include "config/Config.h"
#include "keybdHook/KeybdHook.h"
#include "mouseHook/MouseHook.h"
#include "window/Window.h"
#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  HANDLE hMutex;
  try {
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Textify.exe");
    if (hMutex != nullptr) {
      MessageBox(nullptr, L"应用程序已经在运行!", L"提示", MB_OK);
      return 0;
    }
    hMutex = CreateMutex(nullptr, FALSE, L"Textify.exe");
  } catch (...) {
    return 0;
  }

#ifdef _DEBUG
  if (AllocConsole()) {
    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
  }
#endif
  auto &config_ = Config::Instance();
  if (!config_.init()) { return -1; }

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

  if (hMutex) {
    CloseHandle(hMutex);
    hMutex = nullptr;
  }
  return 0;
}
