#include "MouseHook.h"

HHOOK MouseHook::hMouseHook = nullptr;
Window *MouseHook::win_ = nullptr;
Config *MouseHook::config_ = nullptr;
bool MouseHook::isPress = false;

LRESULT CALLBACK MouseHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    bool mouseHandle = false;
    switch (wParam) {
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
      mouseHandle = true;
      break;
    }

    if (mouseHandle) {
      bool hotkey_down = false;
      bool hotkey_up = false;

      if (bool hotkey_match = checkKeybdState()) {
        switch (wParam) {
        case WM_MBUTTONDOWN: {
          if (const auto flag = checkConfigMouse("mid")) { hotkey_down = true; }
          break;
        }
        case WM_MBUTTONUP: {
          if (const auto flag = checkConfigMouse("mid")) { hotkey_up = true; }
          break;
        }
        case WM_RBUTTONDOWN: {
          if (const auto flag = checkConfigMouse("right")) { hotkey_down = true; }
          break;
        }
        case WM_RBUTTONUP: {
          if (const auto flag = checkConfigMouse("right")) { hotkey_up = true; }
          break;
        }
        }
      }

      // TODO: 增加排除程序
      if (hotkey_down) {
        isPress = true;
        return 1;
      }

      if (hotkey_up && isPress) {
        isPress = false;
        getTextFromPoint();
        return 1;
      }

      isPress = false;
    }
  }
  return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void MouseHook::setGlobalMouseHook(Window *win, Config *config)
{
  win_ = win;
  config_ = config;
  hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(nullptr), 0);
}

void MouseHook::unhookGlobalMouseHook() { UnhookWindowsHookEx(hMouseHook); }

bool MouseHook::checkConfigMouse(const std::string &str)
{
  const auto hotkey = config_->m_config.hotkey;
  if (!hotkey.enable) { return false; }
  if (hotkey.left && str == "left") { return true; }
  if (hotkey.mid && str == "mid") { return true; }
  if (hotkey.right && str == "right") { return true; }
  return false;
}

bool MouseHook::checkKeybdState()
{
  const auto isOK = [](bool hotKey, int vkCode) {
    const SHORT keyState = GetAsyncKeyState(vkCode);
    return hotKey && (keyState & 0x8000);
  };

  const auto hotkey = config_->m_config.hotkey;
  const bool flag = isOK(hotkey.shift, VK_SHIFT) || isOK(hotkey.ctrl, VK_CONTROL) || isOK(hotkey.alt, VK_MENU);
  return flag;
}

int MouseHook::getTextFromPoint()
{
  POINT point;
  if (win_ && GetCursorPos(&point)) {
    std::wstring txt;
    std::vector<size_t> lengths;
    utils::getTextFromPointByUIA(point, txt, lengths);
    win_->show(point, lengths, txt);
    return 1;
  }
  return 1;
}
