#include "windowBase.h"
#include <iostream>

WindowBase::WindowBase() {}
WindowBase::~WindowBase() {}

// 窗口显示
void WindowBase::show()
{
  getMousePos();
  w = 200;
  h = 80;
  SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);
}

// 窗口隐藏
void WindowBase::hide() const { ShowWindow(hwnd, SW_HIDE); }

// 软件退出
void WindowBase::quit() { PostQuitMessage(0); }

// 初始化一个窗口
void WindowBase::initWin()
{
  const auto h_instance = GetModuleHandle(nullptr);
  WNDCLASSEX wcx{};
  wcx.cbSize = sizeof(wcx);
  wcx.style = CS_HREDRAW | CS_VREDRAW;
  wcx.lpfnWndProc = routeWinMsg;
  wcx.cbWndExtra = sizeof(WindowBase *);
  wcx.hInstance = h_instance;
  wcx.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wcx.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
  wcx.lpszClassName = L"Textify";
  if (!RegisterClassEx(&wcx)) {
    MessageBox(nullptr, L"注册窗口类失败", L"系统提示", NULL);
    return;
  }
  hwnd = CreateWindowEx(
    WS_EX_TOOLWINDOW, wcx.lpszClassName, wcx.lpszClassName, WS_POPUP, x, y, w, h, nullptr, nullptr, h_instance, this);
  SetWindowPos(
    hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

// 初始化一个任务栏图标
void WindowBase::initTray() const
{
  NOTIFYICONDATA nid = { sizeof(nid) };
  nid.uID = IDI_ICON1;
  nid.hWnd = hwnd;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = WM_SYSICON;
  nid.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wcscpy_s(nid.szTip, L"Textify v1.0.0");
  Shell_NotifyIcon(NIM_ADD, &nid);
}

// 获取鼠标位置
void WindowBase::getMousePos()
{
  POINT cursorPos;
  GetCursorPos(&cursorPos);
  x = cursorPos.x;
  y = cursorPos.y;
}

LRESULT WindowBase::routeWinMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (msg == WM_NCCREATE) {
    const CREATESTRUCT *pCS = reinterpret_cast<CREATESTRUCT *>(lParam);
    LPVOID pThis = pCS->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
  }
  const auto obj = reinterpret_cast<WindowBase *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  if (obj) {
    if (msg == WM_CLOSE) {
      delete obj;
    } else {
      return obj->winProc(hwnd, msg, wParam, lParam);
    }
  }
  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT WindowBase::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_NCHITTEST: {
    return HTCAPTION;
  }
  case WM_SYSICON: {
    // TODO: 鼠标左键双击打开设置界面
    if (lParam == WM_LBUTTONDBLCLK) {
      show();
      return 0;
    }
    if (lParam == WM_RBUTTONDOWN) {
      POINT cursorPos;
      GetCursorPos(&cursorPos);
      SetForegroundWindow(hwnd);
      const HMENU hMenu = CreatePopupMenu();
      InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_SHOW, L"显示窗口");
      InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_SETTINGS, L"设置");
      InsertMenu(hMenu, -1, MF_SEPARATOR, 0, nullptr);
      InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_EXIT, L"退出");
      TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursorPos.x, cursorPos.y, 0, hwnd, nullptr);
      PostMessage(hwnd, WM_NULL, 0, 0);
      DestroyMenu(hMenu);
    }
  }
    /*case WM_KEYDOWN: {
      if (wParam == VK_ESCAPE) {
        hide();
        return 0;
      }
    }*/
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}
