#pragma once
#include "../../resources/resource.h"
#include <windows.h>

class Window
{
public:
  Window();
  ~Window();

  HWND get() const { return hwnd_; }

  // ��ʼ������
  void initWin();

  // ��ʼ�� tray
  void initTray();

  // ��ʼ�� UI
  void initUI() const;

  // ��ʾ
  void show() const;
  void show(int x, int y, int w, int h) const;

  // ����
  void hide() const;

  // ��������ͼ��˵�
  static HMENU createTrayMenu();

  // �¼�����
  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
  HWND hwnd_;
  WNDCLASS wc_;
  HWND input_;
  NOTIFYICONDATA nid_;

  int w = 160, h = 80;
};
