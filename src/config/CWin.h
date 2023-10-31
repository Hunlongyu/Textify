#pragma once
#include "../../resources/resource.h"
#include "config.h"
#include <Windows.h>
#include <mutex>

#define ID_R_SHIFT 101
#define ID_R_CTRL 102
#define ID_R_ALT 103
#define ID_R_MID 104
#define ID_R_RIGHT 105
#define ID_C_ADMIN 106
#define ID_TITLE 107
#define ID_B_SAVE 108
#define ID_B_CANCEL 109
#define ID_B_DIY 110

class CWin
{
public:
  CWin();
  ~CWin();

  static CWin &Instance();
  static void destroyInstance();

  HWND getHwnd() const { return hwnd_; }

  static void show();

  static void hide();

  static void restartApp();

  static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
  static std::unique_ptr<CWin> instance_;
  static std::mutex mutex_;

  Config::ConfigData config_;

  WNDCLASS wc_;
  static HWND hwnd_;
  HWND hTable;
  HWND hSaveButton;
  HWND hCancelButton;

  static HBRUSH hBrushWhite_;
  static HBRUSH hBrushWhiteBk_;
  HFONT hFont;
  HFONT hFontTitle;


  CWin(const CWin &) = delete;
  CWin &operator=(const CWin &) = delete;

  static void OnSaveButtonClicked();
  static void OnCancelButtonClicked();
  static void OnDIYButtonClicked();

  void initWin();

  void initUI();
};
