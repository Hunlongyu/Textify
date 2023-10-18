#pragma once

#include <Windows.h>

class SingletonWindowClass
{
  HWND hwnd;// 窗口句柄

  NOTIFYICONDATA nid;// 托盘图标的数据

  static SingletonWindowClass *instance;// 单例类的静态实例

  // 静态窗口过程函数
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  // 私有构造函数，防止直接创建对象。
  SingletonWindowClass();

public:
  // 控制访问单实例的静态方法
  static SingletonWindowClass *get();

  // 初始化窗口
  void initWin();

  // 显示窗口
  void show() const;

  // 隐藏窗口
  void hide() const;

  // 销毁窗口
  void dispose() const;

  // 初始化托盘图标
  void initTray();
};
