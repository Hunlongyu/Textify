#pragma once
#include <atlstr.h>
#include <atlwin.h>
#include <minwindef.h>
#include <psapi.h>
#include <vector>

class MouseGlobalHook
{
public:
  MouseGlobalHook(CWindow wndNotify,
    UINT msgNotify,
    int key,
    bool ctrl,
    bool alt,
    bool shift,
    std::vector<CString> excludedPrograms);
  ~MouseGlobalHook();

private:
  static DWORD WINAPI MouseHookThreadProxy(void *p_parameter);
  DWORD MouseHookThread();

  static LRESULT CALLBACK LowLevelMouseProcProxy(int nCode, WPARAM wParam, LPARAM lParam);
  LRESULT LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
  bool IsCursorOnExcludedProgram(POINT pt) const;

  static MouseGlobalHook *volatile m_pThis;
  CWindow m_wndNotify;
  UINT m_msgNotify;
  int m_mouseKey;
  bool m_ctrlKey, m_altKey, m_shiftKey;
  std::vector<CString> m_excludedPrograms;

  volatile HANDLE m_mouseHookThread;
  DWORD m_mouseHookThreadId;
  HANDLE m_mouseHookThreadReadyEvent;
  HHOOK m_lowLevelMouseHook;

  bool m_mouseDownEventHooked = false;
};
