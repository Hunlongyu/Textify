#include "MouseHook.h"

HHOOK MouseHook::hMouseHook = nullptr;
Window *MouseHook::win_ = nullptr;
bool MouseHook::isPress = false;

LRESULT CALLBACK MouseHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0) {
    switch (wParam) {
    case WM_MBUTTONDOWN: {
      const auto flag = checkConfigMouse("mid");
      if (!flag) return 0;
      if (isPress) { break; }
      isPress = true;
      return getTextFromPoint();
    }
    case WM_MBUTTONUP: {
      isPress = false;
      return 1;
    }
    case WM_RBUTTONDOWN: {
      const auto flag = checkConfigMouse("right");
      if (!flag) return 0;
      if (isPress) { break; }
      isPress = true;
      return getTextFromPoint();
    }
    case WM_RBUTTONUP: {
      isPress = false;
      return 1;
    }
    }
  }
  return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void MouseHook::setGlobalMouseHook(Window *win)
{
  MouseHook::win_ = win;
  hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, GetModuleHandle(nullptr), 0);
}

void MouseHook::unhookGlobalMouseHook() { UnhookWindowsHookEx(hMouseHook); }

bool MouseHook::checkConfigMouse(const std::string &str)
{
  const auto hotkey = Config::Instance().m_HotKey;
  if (!hotkey.enable) { return false; }
  if (hotkey.left && str == "left") { return checkKeybdState(); }
  if (hotkey.mid && str == "mid") { return checkKeybdState(); }
  if (hotkey.right && str == "right") { return checkKeybdState(); }
  return false;
}

bool MouseHook::checkKeybdState()
{
  const auto isOK = [](bool hotKey, int vkCode) {
    const SHORT keyState = GetAsyncKeyState(vkCode);
    return hotKey && (keyState & 0x8000);
  };

  const auto hotkey = Config::Instance().m_HotKey;
  const bool flag = isOK(hotkey.shift, VK_SHIFT) || isOK(hotkey.ctrl, VK_CONTROL) || isOK(hotkey.alt, VK_MENU);
  return flag;
}

int MouseHook::getTextFromPoint()
{
  POINT point;
  if (win_ && GetCursorPos(&point)) {
    std::wstring txt;
    CRect rect;
    std::vector<size_t> lengths;
    // utils::getTextFromPointByMSAA(point, txt, rect);
    //  utils::getTextFromPointByUIA(mhs->pt, txt, rect);
    utils::getTextFromPointByUIA(point, txt, rect, lengths);
    // win_->setText(txt);
    //  win_->show(point.x, point.y, rect.Width(), rect.Height());
    win_->show(point, lengths, txt);
    return 1;
  }
  return 1;
}
