#include "Window.h"

#include <iostream>

Window::Window()
{
  const auto &config_ = Config::Instance();
  const auto lists = config_.btn_lists;
  btnCount = lists.size();
  initWin();
  initTray();
  initInput();
  initButtons();
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
      if (hwnd) ShowWindow(hwnd, SW_SHOW);
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
        if (hwnd) ShowWindow(hwnd, SW_HIDE);
        return 0;
      }
      // TODO: 显示设置窗口
      if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
        if (hwnd) ShowWindow(hwnd, SW_SHOW);
        return 0;
      }
      if (LOWORD(wParam) == ID_TRAY_EXIT) {
        PostQuitMessage(0);
        return 0;
      }
    }
    if (HIWORD(wParam) == BN_CLICKED) {
      const int btnId = LOWORD(wParam);
      const auto win = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
      if (win) win->parseBtnHandle(btnId);
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

  w = btnCount > 4 ? btnCount * 22 + 16 + ((btnCount - 1) * 4) : 100;
  h = btnCount > 1 ? 62 : 36;

  hwnd_ = CreateWindowEx(WS_EX_TOPMOST,
    wc_.lpszClassName,
    wc_.lpszClassName,
    WS_POPUP | WS_DLGFRAME,
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

void Window::initInput()
{
  input_ = CreateWindowEx(0,
    L"EDIT",
    L"",
    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN,
    4,
    4,
    w - 16,
    20,
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

void Window::initButtons()
{
  if (!hwnd_) { return; }

  const auto &config_ = Config::Instance();
  const auto lists = config_.btn_lists;

  if (lists.size() <= 0) {
    hasBtn = false;
    return;
  }
  hasBtn = true;

  const auto h_instance = GetModuleHandle(nullptr);
  for (auto i = 0; i < lists.size(); ++i) {
    const auto btn = lists.at(i);
    const HWND btnItem = CreateWindow(L"Button",
      L"Button",
      WS_VISIBLE | WS_CHILD | BS_ICON | BS_FLAT,
      8 + (i * 22) + ((i - 1) * 4),
      20 + 12,
      22,
      22,
      hwnd_,
      reinterpret_cast<HMENU>(i),
      h_instance,
      nullptr);

    HICON btnIcon = static_cast<HICON>(LoadImage(nullptr, btn.icon.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
    SendMessage(btnItem, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(btnIcon));
  }

  SetWindowPos(hwnd_, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void Window::show() const
{
  if (!hwnd_) { return; }
  ShowWindow(hwnd_, SW_SHOW);
}

void Window::show(const POINT &point, const std::vector<size_t> &lengths, const std::wstring &txt)
{
  text = txt;
  const auto count = unicode_character_count(text);
  double iw;
  if (count * 10 < 100) {
    iw = 100;
  } else {
    iw = static_cast<int>(count * 10);
  }
  w = btnCount > 4 ? btnCount * 22 + 16 + ((btnCount - 1) * 4) : 100;
  w = iw > w ? iw : w;
  const double ih = static_cast<int>(lengths.size() + 1) * 20;
  h = btnCount > 0 ? ih + 22 + 20 : ih + 14;

  const int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  const int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  int px = point.x, py = point.y;
  if (point.x + w > screenWidth) { px = screenWidth - w; }
  if (point.y + h > screenHeight) { py = screenHeight - h; }

  SetWindowText(input_, text.c_str());
  SetWindowPos(input_, nullptr, 0, 0, w - 16, ih, SWP_NOZORDER | SWP_NOMOVE);
  SetWindowPos(hwnd_, nullptr, px, py, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
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

size_t Window::unicode_character_count(const std::wstring &str)
{
  size_t count = 0;
  for (size_t i = 0; i < str.size(); ++i) {
    wchar_t ch = str[i];
    if (ch >= 0xD800 && ch <= 0xDBFF)// is high surrogate?
    {
      ++count;
    } else if (!(ch >= 0xDC00 && ch <= 0xDFFF))// if it's not a low surrogate
    {
      ++count;
    }
  }
  return count;
}

void Window::parseBtnHandle(int id)
{
  const auto &config_ = Config::Instance();
  const auto lists = config_.btn_lists;
  const auto btn = lists[id];
  if (btn.type == L"copy") { copyContentToClipboard(); }
}

void Window::copyContentToClipboard() const
{
  std::cout << "copy" << std::endl;
  const int length = GetWindowTextLength(input_) + 1;
  wchar_t *buffer = new wchar_t[length];
  GetWindowText(input_, buffer, length);

  if (OpenClipboard(nullptr)) {
    EmptyClipboard();
    const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, length * sizeof(wchar_t));
    if (hGlobal) {
      void *pGlobal = GlobalLock(hGlobal);
      if (pGlobal) {
        memcpy(pGlobal, buffer, length * sizeof(wchar_t));
        GlobalUnlock(hGlobal);
        SetClipboardData(CF_UNICODETEXT, hGlobal);
      }
    }
    CloseClipboard();
  }
  delete[] buffer;
  hide();
}
