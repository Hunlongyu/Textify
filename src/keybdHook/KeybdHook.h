#pragma once
#include "../window/Window.h"
#include <windows.h>

namespace KeybdHook {
// ȫ����깳�Ӿ��
extern HHOOK hKeybdHook;
extern Window *win_;

// ��괦�������
LRESULT CALLBACK keybdProc(int nCode, WPARAM wParam, LPARAM lParam);

// ��ʼ����ж�ع��ӵĹ���
void setGlobalKeybdHook(Window *win);
void unhookGlobalKeybdHook();

};// namespace KeybdHook
