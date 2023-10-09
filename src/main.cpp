#include "../resource.h"
#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect = { 0, 0, 300, 200 };

    HBRUSH brush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdc, &rect, brush);

    EndPaint(hwnd, &ps);
    return 0;
  }
  case WM_CREATE: {
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.uID = IDI_ICON1;
    nid.hWnd = hwnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER;// hwnd will receive this message when the user clicks the icon
    nid.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
    wcscpy_s(nid.szTip, L"My tray icon");
    Shell_NotifyIcon(NIM_ADD, &nid);
    return 0;
  }
  default: ;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  const wchar_t CLASS_NAME[] = L"My Window Class";
  WNDCLASS wc = {};

  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  if (!RegisterClass(&wc)) {
    MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  //HWND hwnd = CreateWindowEx(0,
  //  CLASS_NAME,
  //  L"My Window",
  //  1,// hide the window
  //  CW_USEDEFAULT,
  //  CW_USEDEFAULT,
  //  CW_USEDEFAULT,
  //  CW_USEDEFAULT,
  //  NULL,
  //  NULL,
  //  hInstance,
  //  NULL);

  HWND hwnd = CreateWindowEx(0,
    CLASS_NAME,
    L"Sample Window",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    200,
    300,
    NULL,
    NULL,
    hInstance,
    NULL);

  if (hwnd == NULL) {
    MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  //ShowWindow(hwnd, SW_HIDE);// Hide the window
  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
