#include "Window.h"

#include <commctrl.h>
#include <iostream>

Window::Window()
{
  config_ = Config::Instance().m_config;
  const auto list = config_.list;
  btnCount = static_cast<int>(list.size());
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
        if (hwnd) ShowWindow(hwnd, SW_SHOW);
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

  const auto list = config_.list;

  if (list.size() <= 0) {
    hasBtn = false;
    return;
  }
  hasBtn = true;

  const auto h_instance = GetModuleHandle(nullptr);
  btns_.clear();
  for (auto i = 0; i < list.size(); ++i) {
    const auto btn = list.at(i);
    const HWND btnItem = CreateWindow(L"Button",
      L"Button",
      WS_VISIBLE | WS_CHILD | BS_ICON | BS_FLAT,
      8 + (i * 22) + ((i - 1) * 4),
      32,
      22,
      22,
      hwnd_,
      reinterpret_cast<HMENU>(i),
      h_instance,
      nullptr);

    HICON btnIcon = static_cast<HICON>(LoadImage(nullptr, btn.icon.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
    SendMessage(btnItem, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(btnIcon));
    btns_.push_back(btnItem);
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
  const int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  const int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  text = txt;
  int iw;
  const auto count = unicode_character_count(text);
  const int iCount = static_cast<int>(count);
  if (iCount * 7 < 100) {
    iw = 100;
  } else {
    iw = iCount * 7;
  }
  w = btnCount > 4 ? btnCount * 22 + 16 + ((btnCount - 1) * 4) : 100;
  w = iw > w ? iw : w;
  if (config_.maxWidth >= 100) { w = w > config_.maxWidth ? config_.maxWidth : w; }

  const int ih = static_cast<int>(lengths.size() + 1) * 21;
  h = btnCount > 0 ? ih + 22 + 20 : ih + 14;

  int px = point.x, py = point.y;
  if (point.x + w > screenWidth) { px = screenWidth - w; }
  if (point.y + h > screenHeight) { py = screenHeight - h; }

  SetWindowText(input_, text.c_str());
  SetWindowPos(input_, nullptr, 0, 0, w - 16, ih, SWP_NOZORDER | SWP_NOMOVE);
  for (auto i = 0; i < btns_.size(); ++i) {
    const auto x = 8 + (i * 22) + ((i - 1) * 4);
    const auto y = ih + 12;
    SetWindowPos(btns_[i], nullptr, x, y, 22, 22, SWP_NOSIZE | SWP_NOZORDER);
  }
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
  const std::wstring delimiter = L"\r\n";
  std::vector<std::wstring> result;
  size_t start = 0;
  size_t end = str.find(delimiter);
  while (end != std::wstring::npos) {
    result.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }
  result.push_back(str.substr(start, end));

  size_t max = 0;
  for (const auto &str : result) {
    size_t half = 0, full = 0;
    for (const auto ch : str) {
      if (ch >= 0x0000 && ch <= 0x007F) {
        ++half;
      } else {
        ++full;
      }
    }
    const size_t count = half + full * 1.9;
    if (count > max) max = count;
  }
  return max;
}

void Window::parseBtnHandle(int id) const
{
  const auto list = config_.list;
  if (id > list.size()) { return; }
  const auto btn = list[id];
  if (btn.type == L"hide") { hide(); }
  if (btn.type == L"copy") { copyContentToClipboard(btn); }
  if (btn.type == L"translate") { openBrowserTranslate(btn); }
  if (btn.type == L"search") { openBrowserSearch(btn); }
}

void Window::copyContentToClipboard(const Config::Btn &btn) const
{
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
  if (btn.hide) { hide(); }
}

std::string Window::urlEncode(const std::string &txt)
{
  constexpr char lookup[] = "0123456789abcdef";
  std::stringstream e;
  for (int i = 0, ix = static_cast<int>(txt.length()); i < ix; i++) {
    const char &c = txt[i];
    if ((48 <= c && c <= 57) ||// 0-9
        (65 <= c && c <= 90) ||// abc...xyz
        (97 <= c && c <= 122) ||// ABC...XYZ
        (c == '-' || c == '_' || c == '.' || c == '~')) {
      e << c;
    } else {
      e << '%';
      e << lookup[(c & 0xF0) >> 4];
      e << lookup[(c & 0x0F)];
    }
  }
  return e.str();
}

std::string Window::WStringToString(const std::wstring &wstr)
{
  int num = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
  char *wide = new char[num];
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, wide, num, NULL, NULL);
  std::string str(wide);
  delete[] wide;
  return str;
}

std::wstring Window::StringToWString(const std::string &str)
{
  int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
  wchar_t *wide = new wchar_t[num];
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
  std::wstring wstr(wide);
  delete[] wide;
  return wstr;
}

void Window::openBrowserTranslate(const Config::Btn &btn) const
{
  const int length = GetWindowTextLength(input_) + 1;
  if (length == 1) { return; }

  const auto buffer = std::make_unique<wchar_t[]>(length);
  GetWindowText(input_, buffer.get(), length);

  const std::wstring wstr(buffer.get());
  auto str = WStringToString(wstr);
  str = urlEncode(str);

  const auto txt = StringToWString(str);
  const auto cmd = btn.command;
  const size_t size = cmd.length() + txt.length() * 2;
  wchar_t *cmdBuffer = new wchar_t[size];
  swprintf(cmdBuffer, size, cmd.c_str(), txt.c_str());
  const std::wstring replacedUrl(cmdBuffer);
  delete[] cmdBuffer;

  ShellExecuteW(nullptr, L"open", replacedUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
  if (btn.hide) { hide(); }
}

void Window::openBrowserSearch(const Config::Btn &btn) const
{
  const int length = GetWindowTextLength(input_) + 1;
  if (length == 1) { return; }

  const auto buffer = std::make_unique<wchar_t[]>(length);
  GetWindowText(input_, buffer.get(), length);

  const std::wstring wstr(buffer.get());
  auto str = WStringToString(wstr);
  str = urlEncode(str);

  const auto txt = StringToWString(str);
  const auto cmd = btn.command;
  const size_t size = cmd.length() + txt.length() * 2;
  wchar_t *cmdBuffer = new wchar_t[size];
  swprintf(cmdBuffer, size, cmd.c_str(), txt.c_str());
  const std::wstring replacedUrl(cmdBuffer);
  delete[] cmdBuffer;

  ShellExecuteW(nullptr, L"open", replacedUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
  if (btn.hide) { hide(); }
}