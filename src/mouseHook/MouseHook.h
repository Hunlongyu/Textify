﻿#pragma once
#include "../config/Config.h"
#include "../utils/Utils.h"
#include "../window/Window.h"

namespace MouseHook {
// 全局鼠标钩子句柄
extern HHOOK hMouseHook;
extern Window *win_;
extern Config *config_;
extern bool isPress;
// 鼠标处理程序函数
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// 初始化和卸载钩子的功能
void setGlobalMouseHook(Window *win, Config *config);
void unhookGlobalMouseHook();

// 检查鼠标点击的程序是否是需要排除的
bool checkExclude();

// 检查配置文件里，有没有注册全局的鼠标快捷键
bool checkConfigMouse(const std::string &str);

// 检查当前键盘几个特殊键是否是按下状态
bool checkKeybdState();

int getTextFromPoint();

};// namespace MouseHook
