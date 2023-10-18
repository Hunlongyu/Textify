﻿#pragma once
#include "../resource.h"
#include <Windows.h>

class WindowBase
{
public:
  WindowBase();
  virtual ~WindowBase();

  void show();
  void hide() const;
  static void quit();
  virtual LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  HWND hwnd;
  int x, y, w, h;
  bool isHotKeyTriggered = false;

protected:
  void initWin();
  void initTray() const;

private:
  void getMousePos();
  static LRESULT CALLBACK routeWinMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};