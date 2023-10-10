#pragma once
#include "WindowBase.h"

class Win : public WindowBase
{
public:
  Win();
  ~Win();

  static void init();
  static Win *get();
  static void dispose();

private:
  bool registerHotKey();
};
