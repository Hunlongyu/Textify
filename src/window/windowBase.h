#pragma once
#include <Windows.h>

class WindowBase
{
public:
  WindowBase();
  virtual ~WindowBase();

  void show() const;
  void hide() const;
  static void quit();

  HWND hwnd;
  int x, y, w, h;

protected:
  void initWin();
  void initTray() const;

private:
  static LRESULT CALLBACK routeWinMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};