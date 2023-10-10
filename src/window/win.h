#pragma once
#include "WindowBase.h"
#include <string>

class Win : public WindowBase
{
public:
  Win();
  ~Win();

  static void init();
  static Win *get();
  static void dispose();

private:
  bool registerHotKey(const std::wstring &key);
};
