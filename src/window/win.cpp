#include "win.h"
#include <algorithm>
#include <sstream>

static Win *win;

Win::Win()
{
  initWin();
  initTray();
  registerHotKey(L"shift");
}

Win::~Win()
{
  UnregisterHotKey(hwnd, 1);
}

void Win::init() { win = new Win(); }
Win *Win::get() { return win; }
void Win::dispose() { delete win; }

// 注册 键盘 全局快捷键
bool Win::registerHotKey(const std::wstring &keys)
{
  std::wistringstream is(keys);
  std::wstring key;
  UINT hotKeyModifiers = 0;

  while (std::getline(is, key, L'+')) {
    std::transform(key.begin(), key.end(), key.begin(), [](wchar_t c) { return std::tolower(c); });

    if (key == L"ctrl") { hotKeyModifiers |= MOD_CONTROL; }
    if (key == L"alt") { hotKeyModifiers |= MOD_ALT; }
    if (key == L"shift") { hotKeyModifiers |= MOD_SHIFT; }
  }

  return RegisterHotKey(nullptr, 1, hotKeyModifiers, 0) != false;
}

