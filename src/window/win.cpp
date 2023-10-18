#include "win.h"

#include <iostream>

static Win *win;

Win::Win()
{
  initWin();
  initTray();
}

Win::~Win() { UninitMouseAndKeyboardHotKeys(); }

bool Win::init()
{
  win = new Win();
  if (!Config::getInstance().load_json_config()) { return false; }
  win->ApplyMouseAndKeyboardHotKeys();
  return true;
}
Win *Win::get() { return win; }

void Win::dispose() { delete win; }

void Win::ApplyMouseAndKeyboardHotKeys()
{
  const auto hotkey = Config::getInstance().m_HotKey;
  if (hotkey.enable) {
    _ATLTRY
    {
      const auto exPrograms = Config::getInstance().m_excludedPrograms;
      m_mouseGlobalHook.emplace(
        hwnd, UWM_MOUSEHOOKCLICKED, hotkey.enable, hotkey.ctrl, hotkey.alt, hotkey.shift, exPrograms);
    }
    _ATLCATCH(e)
    {
      // TODO
    }
  } else {
    m_mouseGlobalHook.reset();
  }

  if (hotkey.enable && !m_registeredHotKey) {
    m_registeredHotKey = RegisterConfiguredKeybdHotKey(hotkey);
    if (!m_registeredHotKey) {
      // TODO
    }
  } else if (!hotkey.enable && m_registeredHotKey) {
    UnregisterHotKey(hwnd, 1);
    m_registeredHotKey = false;
  }
}

bool Win::RegisterConfiguredKeybdHotKey(const HotKey &hotkey) const
{
  UINT mod = 0;
  if (hotkey.ctrl) { mod |= MOD_CONTROL; }
  if (hotkey.alt) { mod |= MOD_ALT; }
  if (hotkey.shift) { mod |= MOD_SHIFT; }
  if (IsWindows7OrGreater()) { mod |= MOD_NOREPEAT; }
  return RegisterHotKey(nullptr, 1, mod, 1) != FALSE;
}

void Win::UninitMouseAndKeyboardHotKeys()
{
  if (m_registeredHotKey) {
    UnregisterHotKey(hwnd, 1);
    m_registeredHotKey = false;
  }
  m_mouseGlobalHook.reset();
}

LRESULT Win::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_COMMAND: {
    if (HIWORD(wParam) == 0) {
      if (LOWORD(wParam) == ID_TRAY_SHOW) {
        show();
        return 0;
      }
      // TODO: 显示设置窗口
      if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
        show();
        return 0;
      }
      if (LOWORD(wParam) == ID_TRAY_EXIT) {
        quit();
        return 0;
      }
    }
  }
  case WM_KEYDOWN: {
    if (wParam == VK_ESCAPE) {
      hide();
      return 0;
    }
  }
  case WM_HOTKEY: {
    std::cout << wParam << lParam << std::endl;

    const auto hky = HIWORD(wParam);
    std::cout << hky << std::endl;
    const auto lky = LOWORD(wParam);
    std::cout << lky << std::endl;

    const auto hvk = HIWORD(lParam);
    std::cout << hvk << std::endl;
    const auto lvk = LOWORD(lParam);
    std::cout << lvk << std::endl;
    if (lvk == 10086) {
      isHotKeyTriggered = true;
      show();
      // return 0;
    }
    // break;
    return 0;
  }
  case WM_RBUTTONDOWN: {
    // show();
    return 0;
  }
  }
  return WindowBase::winProc(hwnd, msg, wParam, lParam);
}
