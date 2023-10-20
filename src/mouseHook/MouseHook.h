#pragma once
#include "../config/Config.h"
#include "../window/Window.h"
#include <windows.h>

namespace MouseHook {
// ȫ����깳�Ӿ��
extern HHOOK hMouseHook;
extern Window *win_;

// ��괦�������
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// ��ʼ����ж�ع��ӵĹ���
void setGlobalMouseHook(Window *win);
void unhookGlobalMouseHook();

// ��������ļ����û��ע��ȫ�ֵ�����ݼ�
bool checkConfigMouse(std::string str);

// ��鵱ǰ���̼���������Ƿ��ǰ���״̬
bool checkKeybdState();

};// namespace MouseHook
