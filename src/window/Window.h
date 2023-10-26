#pragma once
#include "../../resources/resource.h"
#include "../config/Config.h"
#include <string>
#include <vector>
#include <windows.h>

class Window
{
public:
  Window();
  ~Window();

  HWND get() const { return hwnd_; }

  // 初始化窗口
  void initWin();

  // 初始化 tray
  void initTray();

  // 初始化 input
  void initInput();

  // 初始化 button、
  void initButtons();

  // 显示
  void show() const;
  void show(const POINT &point, const std::vector<size_t> &lengths, const std::wstring &txt);

  // 隐藏
  void hide() const;

  // 创建托盘图标菜单
  static HMENU createTrayMenu();

  // 事件管理
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
  HWND hwnd_;
  WNDCLASS wc_;
  HWND input_;
  NOTIFYICONDATA nid_;

  int w = 108, h = 60;// 窗口宽高
  bool hasBtn{ false };
  int btnCount{ 0 };

  std::wstring text;// 识别到的文字

  // 获取文字个数
  static size_t unicode_character_count(const std::wstring &str);

  // 解析按钮事件
  void parseBtnHandle(int);

  // 复制输入框内容到剪贴板
  void copyContentToClipboard() const;
};
