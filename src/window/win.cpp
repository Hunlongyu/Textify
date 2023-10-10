#include "win.h"

static Win *win;

Win::Win()
{
  initWinSize();
  initWin();
  initTray();
}

Win::~Win() {}
void Win::init() { win = new Win(); }
Win *Win::get() { return win; }
void Win::dispose() { delete win; }

// 初始化窗口大小
void Win::initWinSize()
{
  /*x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  h = GetSystemMetrics(SM_CYVIRTUALSCREEN);*/
  x = 10;
  y = 10;
  w = 300;
  h = 80;
}
