#pragma once

#include <Windows.h>

class WinBase
{
public:
  WinBase();
  virtual ~WinBase();

  void show();
  void hide();
  void quit();
};