#include "win.h"
#include "../config/config.h"
#include <VersionHelpers.h>

static Win *win;

Win::Win()
{
  initWin();
  initTray();
  registerHotKey();
}

Win::~Win() { UnregisterHotKey(hwnd, 1); }

void Win::init() { win = new Win(); }
Win *Win::get() { return win; }
void Win::dispose() { delete win; }

// 注册 键盘 全局快捷键
bool Win::registerHotKey()
{
  const auto hotkey = Config::getInstance().m_HotKey;
  UINT modifiers = 0;
  if (hotkey.shift) { modifiers |= MOD_SHIFT; }
  if (hotkey.alt) { modifiers |= MOD_ALT; }
  if (hotkey.ctrl) { modifiers |= MOD_CONTROL; }

  // Set MOD_NOREPEAT only for Windows 7 and newer.
  if (IsWindows7OrGreater()) { modifiers |= 0x4000; }

  return RegisterHotKey(hwnd, 1, modifiers, 0) != FALSE;
}
