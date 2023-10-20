#pragma once
#include "../window/Window.h"
#include <windows.h>

namespace KeybdHook {
// 全局鼠标钩子句柄
extern HHOOK hKeybdHook;
extern Window *win_;

// 鼠标处理程序函数
LRESULT CALLBACK keybdProc(int nCode, WPARAM wParam, LPARAM lParam);

// 初始化和卸载钩子的功能
void setGlobalKeybdHook(Window *win);
void unhookGlobalKeybdHook();

};// namespace KeybdHook
