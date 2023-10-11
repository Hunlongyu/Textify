#include "config/config.h"
#include "window/win.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  if (!Config::getInstance().load_json_config()) { return -1; }

  Win::init();

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  Win::dispose();
  return 0;
}