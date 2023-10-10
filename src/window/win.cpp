#include "win.h"

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
  // return RegisterHotKey(nullptr, 1, hotKeyModifiers, 0) != false;
  return false;
}
