#include "CWin.h"

#include <commctrl.h>

std::unique_ptr<CWin> CWin::instance_ = nullptr;
std::mutex CWin::mutex_;
HWND CWin::hwnd_ = nullptr;
HBRUSH CWin::hBrushWhite_ = nullptr;
HBRUSH CWin::hBrushWhiteBk_ = nullptr;

CWin::CWin()
{
  config_ = Config::Instance().m_config;
  hBrushWhite_ = CreateSolidBrush(RGB(255, 255, 255));
  hBrushWhiteBk_ = CreateSolidBrush(RGB(255, 255, 255));
  initWin();
  initUI();
}

CWin::~CWin()
{
  DeleteObject(hFont);
  DeleteObject(hFontTitle);
  DeleteObject(hBrushWhite_);
  DeleteObject(hBrushWhiteBk_);
}

CWin &CWin::Instance()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) { instance_ = std::make_unique<CWin>(); }
  return *instance_.get();
}

void CWin::destroyInstance()
{
  std::lock_guard<std::mutex> lock(mutex_);
  instance_.reset(nullptr);
}

void CWin::initWin()
{
  const auto h_instance = GetModuleHandle(nullptr);
  wc_ = { 0 };
  wc_.style = CS_HREDRAW | CS_VREDRAW;
  wc_.lpfnWndProc = WndProc;
  wc_.hInstance = h_instance;
  wc_.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
  wc_.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
  wc_.lpszClassName = L"设置";

  if (!RegisterClass(&wc_)) {
    MessageBox(nullptr, L"注册窗口类失败", L"系统提示", NULL);
    return;
  }

  hwnd_ = CreateWindowEx(WS_EX_TOPMOST,
    wc_.lpszClassName,
    wc_.lpszClassName,
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    100,
    100,
    365,
    236,
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

void CWin::initUI()
{
  hFont = CreateFont(18,
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
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    L"Microsoft YaHei");
  hFontTitle = CreateFont(22,
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
    DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    L"Microsoft YaHei");

  HWND hw;
  hw = CreateWindow(
    L"STATIC", L"组合快捷键", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 10, 200, 20, hwnd_, (HMENU)ID_TITLE, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFontTitle, TRUE);

  hw = CreateWindow(L"BUTTON",
    L"Shift 键",
    WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
    10,
    40,
    80,
    30,
    hwnd_,
    (HMENU)ID_R_SHIFT,
    NULL,
    NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(L"BUTTON",
    L"Ctrl 键",
    WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
    100,
    40,
    80,
    30,
    hwnd_,
    (HMENU)ID_R_CTRL,
    NULL,
    NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"Alt 键", WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON, 190, 40, 80, 30, hwnd_, (HMENU)ID_R_ALT, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"鼠标中键", WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON, 10, 70, 80, 30, hwnd_, (HMENU)ID_R_MID, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(L"BUTTON",
    L"鼠标右键",
    WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
    100,
    70,
    80,
    30,
    hwnd_,
    (HMENU)ID_R_RIGHT,
    NULL,
    NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(L"BUTTON",
    L"管理员权限启动",
    WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
    10,
    130,
    120,
    30,
    hwnd_,
    (HMENU)ID_C_ADMIN,
    NULL,
    NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"保存", WS_VISIBLE | WS_CHILD | BS_FLAT, 140, 130, 60, 30, hwnd_, (HMENU)ID_B_SAVE, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"取消", WS_VISIBLE | WS_CHILD | BS_FLAT, 210, 130, 60, 30, hwnd_, (HMENU)ID_B_CANCEL, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"高级", WS_VISIBLE | WS_CHILD | BS_FLAT, 280, 130, 60, 30, hwnd_, (HMENU)ID_B_DIY, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"STATIC", L"版本：v1.0.0", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 170, 90, 20, hwnd_, (HMENU)ID_VERSION, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"STATIC", L"项目地址：", WS_VISIBLE | WS_CHILD | SS_LEFT, 220, 170, 60, 20, hwnd_, (HMENU)ID_VERSION, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  hw = CreateWindow(
    L"BUTTON", L"Github", WS_VISIBLE | WS_CHILD | BS_FLAT, 280, 170, 60, 16, hwnd_, (HMENU)ID_GITHUB, NULL, NULL);
  SendMessage(hw, WM_SETFONT, (WPARAM)hFont, TRUE);

  if (config_.hotkey.shift) { SendMessage(GetDlgItem(hwnd_, ID_R_SHIFT), BM_SETCHECK, BST_CHECKED, 0); }
  if (config_.hotkey.ctrl) { SendMessage(GetDlgItem(hwnd_, ID_R_CTRL), BM_SETCHECK, BST_CHECKED, 0); }
  if (config_.hotkey.alt) { SendMessage(GetDlgItem(hwnd_, ID_R_ALT), BM_SETCHECK, BST_CHECKED, 0); }

  if (config_.hotkey.mid) { SendMessage(GetDlgItem(hwnd_, ID_R_MID), BM_SETCHECK, BST_CHECKED, 0); }
  if (config_.hotkey.right) { SendMessage(GetDlgItem(hwnd_, ID_R_RIGHT), BM_SETCHECK, BST_CHECKED, 0); }

  if (config_.admin) { SendMessage(GetDlgItem(hwnd_, ID_C_ADMIN), BM_SETCHECK, BST_CHECKED, 0); }
}

void CWin::restartApp()
{
  MessageBox(nullptr, L"软件将保存配置并退出，请自行重启。", L"提示", NULL);
  PostQuitMessage(0);
}

void CWin::show()
{
  if (!hwnd_) { return; }
  if (IsIconic(hwnd_)) {
    ShowWindow(hwnd_, SW_RESTORE);
  } else {
    ShowWindow(hwnd_, SW_SHOW);
  }
}

void CWin::hide()
{
  if (!hwnd_) { return; }
  ShowWindow(hwnd_, SW_HIDE);
}

LRESULT CWin::WndProc(HWND hwnd_, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_CLOSE: {
    ShowWindow(hwnd_, SW_HIDE);
    break;
  }
  case WM_COMMAND: {
    auto win = reinterpret_cast<CWin *>(GetWindowLongPtr(hwnd_, GWLP_USERDATA));
    switch (LOWORD(wParam)) {
    case ID_R_SHIFT: {
      CheckRadioButton(hwnd_, ID_R_SHIFT, ID_R_ALT, LOWORD(wParam));
      win->config_.hotkey.shift = true;
      Config::Instance().change("hotkey.shift", true);
      win->config_.hotkey.ctrl = false;
      Config::Instance().change("hotkey.ctrl", false);
      win->config_.hotkey.alt = false;
      Config::Instance().change("hotkey.alt", false);
      break;
    }
    case ID_R_CTRL: {
      CheckRadioButton(hwnd_, ID_R_SHIFT, ID_R_ALT, LOWORD(wParam));
      win->config_.hotkey.shift = false;
      Config::Instance().change("hotkey.shift", false);
      win->config_.hotkey.ctrl = true;
      Config::Instance().change("hotkey.ctrl", true);
      win->config_.hotkey.alt = false;
      Config::Instance().change("hotkey.alt", false);
      break;
    }
    case ID_R_ALT: {
      CheckRadioButton(hwnd_, ID_R_SHIFT, ID_R_ALT, LOWORD(wParam));
      win->config_.hotkey.shift = false;
      Config::Instance().change("hotkey.shift", false);
      win->config_.hotkey.ctrl = false;
      Config::Instance().change("hotkey.ctrl", false);
      win->config_.hotkey.alt = true;
      Config::Instance().change("hotkey.alt", true);
      break;
    }
    case ID_R_MID: {
      CheckRadioButton(hwnd_, ID_R_MID, ID_R_RIGHT, LOWORD(wParam));
      win->config_.hotkey.mid = true;
      Config::Instance().change("hotkey.mid", true);
      win->config_.hotkey.right = false;
      Config::Instance().change("hotkey.right", false);
      break;
    }
    case ID_R_RIGHT: {
      CheckRadioButton(hwnd_, ID_R_MID, ID_R_RIGHT, LOWORD(wParam));
      win->config_.hotkey.mid = false;
      Config::Instance().change("hotkey.mid", false);
      win->config_.hotkey.right = true;
      Config::Instance().change("hotkey.right", true);
      break;
    }
    case ID_C_ADMIN: {
      const LRESULT isChecked = SendMessage(GetDlgItem(hwnd_, ID_C_ADMIN), BM_GETCHECK, 0, 0);
      const bool flag = (isChecked == BST_CHECKED);
      Config::Instance().change("admin", flag);
      break;
    }
    case ID_B_SAVE: {
      OnSaveButtonClicked();
      break;
    }
    case ID_B_CANCEL: {
      OnCancelButtonClicked();
      break;
    }
    case ID_B_DIY: {
      OnDIYButtonClicked();
      break;
    }
    case ID_GITHUB: {
      ShellExecute(NULL, L"open", L"https://github.com/Hunlongyu/Textify", NULL, NULL, SW_SHOWNORMAL);
    }
    }
    break;
  }
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORSTATIC: {
    auto hdc = (HDC)wParam;
    SetBkMode(hdc, TRANSPARENT);
    return (LONG)hBrushWhite_;
  }
  default:
    return DefWindowProc(hwnd_, msg, wParam, lParam);
  }

  return 0;
}

void CWin::OnSaveButtonClicked()
{
  const auto flag = Config::Instance().save();
  if (flag) { restartApp(); }
}
void CWin::OnCancelButtonClicked() { hide(); }

void CWin::OnDIYButtonClicked()
{
  const std::string currentDir = ".\\";
  ShellExecuteA(hwnd_, "open", "explorer.exe", currentDir.c_str(), nullptr, SW_SHOWDEFAULT);
}
