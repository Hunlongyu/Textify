#include "KeybdHook.h"

HHOOK KeybdHook::hKeybdHook = nullptr;
Window *KeybdHook::win_ = nullptr;

LRESULT CALLBACK KeybdHook::keybdProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
      KBDLLHOOKSTRUCT *pKeybdHook = (KBDLLHOOKSTRUCT *)lParam;
      if (pKeybdHook->vkCode == VK_ESCAPE && win_) {// 检测到ESC键按下
        win_->hide();// 隐藏窗口
      }
    }
  }
  return CallNextHookEx(hKeybdHook, nCode, wParam, lParam);
}

void KeybdHook::setGlobalKeybdHook(Window *win)
{
  win_ = win;
  hKeybdHook = SetWindowsHookEx(WH_KEYBOARD_LL, keybdProc, GetModuleHandle(NULL), 0);
}

void KeybdHook::unhookGlobalKeybdHook() { UnhookWindowsHookEx(hKeybdHook); }