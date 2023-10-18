#pragma once
#include "../config/config.h"
#include "../hook/MouseGlobalHook/MouseGlobalHook.h"
#include "WindowBase.h"
#include "optional"
#include <VersionHelpers.h>

class Win : public WindowBase
{
public:
  enum {
    UWM_MOUSEHOOKCLICKED = WM_APP,
    UWM_NOTIFYICON,
    UWM_BRING_TO_FRONT,
    UWM_UPDATE_CHECKED,
    UWM_EXIT,
  };

  Win();
  ~Win();

  static bool init();
  static Win *get();
  static void dispose();

  bool m_registeredHotKey = false;// 是否注册快捷键
  std::optional<MouseGlobalHook> m_mouseGlobalHook;// 鼠标全局钩子

  void ApplyMouseAndKeyboardHotKeys();
  bool RegisterConfiguredKeybdHotKey(const HotKey &keybdHotKey) const;
  void UninitMouseAndKeyboardHotKeys();
  LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};
