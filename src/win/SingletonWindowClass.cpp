#include "SingletonWindowClass.h"
#include "../resource.h"

SingletonWindowClass *SingletonWindowClass::instance = nullptr;

LRESULT SingletonWindowClass::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_CLOSE: {
    DestroyWindow(hwnd);
    break;
  }
  case WM_DESTROY: {
    Shell_NotifyIcon(NIM_DELETE, &instance->nid);// 清理托盘图标
    PostQuitMessage(0);
    break;
  }
  default: {
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  }
  DefWindowProc(hwnd, msg, wParam, lParam);
}

SingletonWindowClass::SingletonWindowClass()
{
  // initWin();
  initTray();
}

SingletonWindowClass *SingletonWindowClass::get()
{
  if (instance == nullptr) { instance = new SingletonWindowClass; }
  return instance;
}

// 初始化窗口
void SingletonWindowClass::initWin()
{
  // WNDCLASSEX wc{};
  // wc.cbSize = sizeof(wc);
  // wc.style = CS_HREDRAW | CS_VREDRAW;
  //// wc.lpfnWndProc = routeWinMsg;
  //// wc.cbWndExtra = sizeof(WindowBase *);
  //// wc.hInstance = h_instance;
  // wc.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  // wc.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
  // wc.lpszClassName = L"Textify";

  WNDCLASSEX wc;
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wc.hIconSm = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wc.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
  wc.lpszClassName = L"Textify";

  if (!RegisterClassEx(&wc)) {
    DWORD errCode = GetLastError();
    TCHAR errMsg[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      errCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),//  Default language
      errMsg,
      sizeof(errMsg) / sizeof(TCHAR),
      NULL);
    MessageBox(NULL, errMsg, TEXT("Error"), MB_OK);
    return;
  }

  /*hwnd = CreateWindowEx(
    WS_EX_TOOLWINDOW, wc.lpszClassName, wc.lpszClassName, WS_POPUP, x, y, w, h, nullptr, nullptr, h_instance, this);*/

  hwnd = CreateWindowEx(WS_EX_TOOLWINDOW,
    wc.lpszClassName,
    wc.lpszClassName,
    WS_POPUP,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    240,
    120,
    nullptr,
    nullptr,
    GetModuleHandle(nullptr),
    nullptr);

  if (hwnd == nullptr) { return; }
}

// 窗口显示
void SingletonWindowClass::show() const { ShowWindow(hwnd, SW_SHOW); }

// 窗口隐藏
void SingletonWindowClass::hide() const { ShowWindow(hwnd, SW_HIDE); }

// 窗口销毁
void SingletonWindowClass::dispose() const { DestroyWindow(hwnd); }

// 初始化托盘图标
void SingletonWindowClass::initTray()
{
  ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = WM_SYSICON;
  nid.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wcscpy_s(nid.szTip, L"Textify v1.0.0");
  Shell_NotifyIcon(NIM_ADD, &nid);
}
