#include "Window.h"

#include <iostream>

Window::Window()
{
  initWin();
  initTray();
  initUI();
}

Window::~Window() {}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_NCHITTEST: {
    return HTCAPTION;
  }
  case WM_CLOSE: {
    PostQuitMessage(0);
    break;
  }
  case WM_SYSICON: {
    // TODO: 鼠标左键双击打开设置界面
    if (lParam == WM_LBUTTONDBLCLK) {
      const auto win = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
      win->show();
      return 0;
    }
    if (lParam == WM_RBUTTONDOWN) {
      POINT cursorPos;
      GetCursorPos(&cursorPos);
      SetForegroundWindow(hwnd);
      const HMENU hMenu = createTrayMenu();
      TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursorPos.x, cursorPos.y, 0, hwnd, nullptr);
      PostMessage(hwnd, WM_NULL, 0, 0);
      DestroyMenu(hMenu);
    }
  }
  case WM_COMMAND: {
    if (HIWORD(wParam) == 0) {
      if (LOWORD(wParam) == ID_TRAY_SHOW) {
        const auto win = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (win) win->show();
        return 0;
      }
      // TODO: 显示设置窗口
      if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
        const auto win = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (win) win->show();
        return 0;
      }
      if (LOWORD(wParam) == ID_TRAY_EXIT) {
        PostQuitMessage(0);
        return 0;
      }
    }
  }
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  return 0;
}

void Window::initWin()
{
  const auto h_instance = GetModuleHandle(nullptr);
  wc_ = { 0 };
  wc_.style = CS_HREDRAW | CS_VREDRAW;
  wc_.lpfnWndProc = WndProc;
  wc_.hInstance = h_instance;
  wc_.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
  wc_.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wc_.lpszClassName = L"Textify";

  if (!RegisterClass(&wc_)) {
    MessageBox(nullptr, L"注册窗口类失败", L"系统提示", NULL);
    return;
  }

  hwnd_ = CreateWindowEx(WS_EX_TOPMOST,
    wc_.lpszClassName,
    wc_.lpszClassName,
    WS_POPUP | WS_CLIPCHILDREN,
    100,
    100,
    w,
    h,
    nullptr,
    nullptr,
    h_instance,
    this);
  if (hwnd_ == nullptr) {
    MessageBox(nullptr, L"注册窗口类失败", L"系统提示", NULL);
    return;
  }
  SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void Window::initTray()
{
  nid_.cbSize = sizeof(NOTIFYICONDATA);
  nid_.uID = IDI_ICON1;
  nid_.hWnd = hwnd_;
  nid_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid_.uCallbackMessage = WM_SYSICON;
  nid_.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wcscpy_s(nid_.szTip, L"Textify v1.0.0");
  Shell_NotifyIcon(NIM_ADD, &nid_);
}

void Window::initUI()
{
  input_ = CreateWindowEx(0,
    L"EDIT",
    L"",
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN,
    10,
    10,
    w - 20,
    (h - 20) - 20 - 4,
    hwnd_,
    reinterpret_cast<HMENU>(IDC_INPUT),
    nullptr,
    nullptr);
  if (input_) {
    HFONT hFont = CreateFont(-MulDiv(10, GetDeviceCaps(GetDC(input_), LOGPIXELSY), 72),
      0,
      0,
      0,
      FW_NORMAL,
      FALSE,
      FALSE,
      FALSE,
      DEFAULT_CHARSET,
      OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS,
      CLEARTYPE_QUALITY,
      VARIABLE_PITCH,
      L"Microsoft YaHei");
    if (hFont) { SendMessage(input_, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE); }
  }
}

void Window::show() const
{
  if (!hwnd_) { return; }
  ShowWindow(hwnd_, SW_SHOW);
}

void Window::show(const int x, const int y, const int w, const int h) const
{
  const int width = w + 40;
  const int height = h + 40;
  // SetWindowPos(input_, hwnd_, 10, 10, w, h, true);
  MoveWindow(input_,// handle to window
    10,// new x position
    10,// new y position
    w,// new width
    h,// new height
    TRUE// should the window be repainted
  );
  SetWindowPos(hwnd_, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  ShowWindow(hwnd_, SW_SHOW);
  UpdateWindow(hwnd_);
}

void Window::show(const POINT &point, const std::vector<size_t> &lengths, const std::wstring &txt)
{
  text = txt;
  text += L" point: ";
  text += std::to_wstring(point.x);
  text += L" , ";
  text += std::to_wstring(point.y);
  SetWindowText(input_, text.c_str());
  SetWindowPos(hwnd_, nullptr, point.x, point.y, w + 200, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  ShowWindow(hwnd_, SW_SHOW);
}


void Window::hide() const
{
  if (!hwnd_) { return; }
  ShowWindow(hwnd_, SW_HIDE);
}

HMENU Window::createTrayMenu()
{
  const HMENU menu = CreatePopupMenu();
  InsertMenu(menu, -1, MF_BYPOSITION, ID_TRAY_SHOW, L"显示窗口");
  InsertMenu(menu, -1, MF_BYPOSITION, ID_TRAY_SETTINGS, L"设置");
  InsertMenu(menu, -1, MF_SEPARATOR, 0, nullptr);
  InsertMenu(menu, -1, MF_BYPOSITION, ID_TRAY_EXIT, L"退出");
  return menu;
}

void Window::setText(const std::wstring &str)
{
  text = str;
  SetWindowText(input_, str.c_str());
  // UpdateWindow(input_);
}
