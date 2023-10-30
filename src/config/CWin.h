#pragma once
#include "../../resources/resource.h"
#include <Windows.h>
#include <mutex>

class CWin
{
public:
  CWin();
  ~CWin();

  static CWin &Instance();
  static void destroyInstance();

  HWND getHwnd() const { return hwnd_; }

  void show() const;

  void hide() const;

  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
  static std::unique_ptr<CWin> instance_;
  static std::mutex mutex_;

  WNDCLASS wc_;
  static HWND hwnd_;
  HWND hTable;
  HWND hSaveButton;
  HWND hCancelButton;

  CWin(const CWin &) = delete;
  CWin &operator=(const CWin &) = delete;

  static void OnSaveButtonClicked();
  static void OnCancelButtonClicked();

  void initWin();

  void initTable();

  void initButtons();
};
