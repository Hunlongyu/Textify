#pragma once

#include <Windows.h>

class SingletonWindowClass
{
  HWND hwnd;// ���ھ��

  NOTIFYICONDATA nid;// ����ͼ�������

  static SingletonWindowClass *instance;// ������ľ�̬ʵ��

  // ��̬���ڹ��̺���
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

  // ˽�й��캯������ֱֹ�Ӵ�������
  SingletonWindowClass();

public:
  // ���Ʒ��ʵ�ʵ���ľ�̬����
  static SingletonWindowClass *get();

  // ��ʼ������
  void initWin();

  // ��ʾ����
  void show() const;

  // ���ش���
  void hide() const;

  // ���ٴ���
  void dispose() const;

  // ��ʼ������ͼ��
  void initTray();
};
