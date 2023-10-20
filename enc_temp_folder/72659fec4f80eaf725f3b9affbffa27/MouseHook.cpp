#include "MouseHook.h"

HHOOK MouseHook::hMouseHook = nullptr;
Window *MouseHook::win_ = nullptr;
bool MouseHook::isPress = false;

LRESULT CALLBACK MouseHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0) {
    switch (wParam) {
    case WM_LBUTTONDOWN: {
      const auto flag = checkConfigMouse("left");
      if (!flag) { break; }
      const MOUSEHOOKSTRUCT *mhs = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
      if (win_) { win_->show(mhs->pt.x, mhs->pt.y, 200, 80); }
      break;
    }
    case WM_MBUTTONDOWN: {
      const auto flag = checkConfigMouse("mid");
      if (!flag) { break; }
      const MOUSEHOOKSTRUCT *mhs = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
      if (win_) {
        win_->setText(L"恭喜发财");
        win_->show(mhs->pt.x, mhs->pt.y, 200, 80);
      }
      break;
    }
    case WM_RBUTTONDOWN: {
      if (isPress) { break; }
      isPress = true;
      const auto flag = checkConfigMouse("right");
      if (!flag) { break; }
      const MOUSEHOOKSTRUCT *mhs = reinterpret_cast<MOUSEHOOKSTRUCT *>(lParam);
      if (mhs->hwnd == win_->get()) { break; }
      if (win_) {
        std::wstring txt;
        CRect rect;
        utils::getTextFromPointByMSAA(mhs->pt, txt, rect);
        win_->setText(txt);
        win_->show(mhs->pt.x, mhs->pt.y, 200, 80);
      }
      break;
    }
    case WM_RBUTTONUP: {
      isPress = false;
      break;
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

bool MouseHook::checkConfigMouse(std::string pos)
{
  const auto hotkey = Config::Instance().m_HotKey;
  if (!hotkey.enable) { return false; }
  if (hotkey.left && pos == "left") { return checkKeybdState(); }
  if (hotkey.mid && pos == "mid") { return checkKeybdState(); }
  if (hotkey.right && pos == "right") { return checkKeybdState(); }
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
