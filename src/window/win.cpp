#include "win.h"

static Win *win;

Win::Win()
{
  initWin();
  initTray();
}

Win::~Win() {}

void Win::init() { win = new Win(); }
Win *Win::get() { return win; }
void Win::dispose() { delete win; }
