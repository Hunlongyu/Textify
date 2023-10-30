#include "CWin.h"

std::unique_ptr<CWin> CWin::instance_ = nullptr;
std::mutex CWin::mutex_;
HWND CWin::hwnd_ = nullptr;

CWin::CWin() { initWin(); }

CWin::~CWin() {}


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
  wc_.lpszClassName = L"Config";

  if (!RegisterClass(&wc_)) {
    MessageBox(nullptr, L"注册窗口类失败", L"系统提示", NULL);
    return;
  }

  hwnd_ = CreateWindowEx(WS_EX_TOPMOST,
    wc_.lpszClassName,
    wc_.lpszClassName,
    WS_OVERLAPPEDWINDOW,
    100,
    100,
    500,
    400,
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

void CWin::initTable() {}
void CWin::initButtons() {}

void CWin::show() const
{
  if (!hwnd_) { return; }
  ShowWindow(hwnd_, SW_SHOW);
}

void CWin::hide() const
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
  default:
    return DefWindowProc(hwnd_, msg, wParam, lParam);
  }

  return 0;
}

void CWin::OnSaveButtonClicked() {}
void CWin::OnCancelButtonClicked() {}
