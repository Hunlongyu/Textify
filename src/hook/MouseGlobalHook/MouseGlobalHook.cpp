#include "MouseGlobalHook.h"

MouseGlobalHook *volatile MouseGlobalHook::m_pThis;

MouseGlobalHook::MouseGlobalHook(CWindow wndNotify,
  UINT msgNotify,
  int key,
  bool ctrl,
  bool alt,
  bool shift,
  std::vector<CString> excludedPrograms)
{
  if (_InterlockedCompareExchangePointer(reinterpret_cast<void *volatile *>(&m_pThis), this, nullptr)) {
    AtlThrow(E_FAIL);
  }

  CHandle readyEvent{ CreateEvent(nullptr, TRUE, FALSE, nullptr) };
  if (!readyEvent) { AtlThrow(AtlHresultFromLastError()); }
  m_mouseHookThreadReadyEvent = readyEvent;

  m_mouseHookThread = CreateThread(nullptr, 0, MouseHookThreadProxy, this, CREATE_SUSPENDED, &m_mouseHookThreadId);
  if (!m_mouseHookThread) { AtlThrow(AtlHresultFromLastError()); }

  SetThreadPriority(m_mouseHookThread, THREAD_PRIORITY_TIME_CRITICAL);
  ResumeThread(m_mouseHookThread);
  WaitForSingleObject(readyEvent, INFINITE);
}

MouseGlobalHook::~MouseGlobalHook()
{
  const HANDLE h_thread = _InterlockedExchangePointer(&m_mouseHookThread, nullptr);
  if (h_thread) {
    PostThreadMessage(m_mouseHookThreadId, WM_APP, 0, 0);
    WaitForSingleObject(h_thread, INFINITE);
    CloseHandle(h_thread);
  }
  m_pThis = nullptr;
}

DWORD WINAPI MouseGlobalHook::MouseHookThreadProxy(void *p_parameter)
{
  const auto p_this = static_cast<MouseGlobalHook *>(p_parameter);
  return p_this->MouseHookThread();
}

DWORD MouseGlobalHook::MouseHookThread()
{
  MSG msg;
  PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
  SetEvent(m_mouseHookThreadReadyEvent);
  m_mouseHookThreadReadyEvent = nullptr;

  m_lowLevelMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProcProxy, GetModuleHandle(nullptr), 0);
  if (m_lowLevelMouseHook) {
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      if (bRet == -1) {
        msg.wParam = 0;
        break;
      }

      if (msg.hwnd == nullptr && msg.message == WM_APP) {
        PostQuitMessage(0);
        continue;
      }

      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(m_lowLevelMouseHook);
  } else {
    msg.wParam = 0;
  }

  const HANDLE hThread = _InterlockedExchangePointer(&m_mouseHookThread, nullptr);
  if (hThread) { CloseHandle(hThread); }

  return static_cast<DWORD>(msg.wParam);
}

LRESULT CALLBACK MouseGlobalHook::LowLevelMouseProcProxy(int nCode, WPARAM wParam, LPARAM lParam)
{
  return m_pThis->LowLevelMouseProc(nCode, wParam, lParam);
}

LRESULT MouseGlobalHook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    bool handleEvent = false;

    switch (wParam) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
      handleEvent = true;
      break;
    }

    if (handleEvent) {
      MSLLHOOKSTRUCT *msllHookStruct = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam);

      bool hotkeyDownDetected = false;
      bool hotkeyUpDetected = false;

      bool bModifiersMatch = (GetKeyState(VK_CONTROL) < 0) == m_ctrlKey && (GetKeyState(VK_MENU) < 0) == m_altKey
                             && (GetKeyState(VK_SHIFT) < 0) == m_shiftKey;

      if (bModifiersMatch) {
        switch (wParam) {
        case WM_LBUTTONDOWN:
          if (m_mouseKey == VK_LBUTTON) hotkeyDownDetected = true;
          break;

        case WM_LBUTTONUP:
          if (m_mouseKey == VK_LBUTTON) hotkeyUpDetected = true;
          break;

        case WM_RBUTTONDOWN:
          if (m_mouseKey == VK_RBUTTON) hotkeyDownDetected = true;
          break;

        case WM_RBUTTONUP:
          if (m_mouseKey == VK_RBUTTON) hotkeyUpDetected = true;
          break;

        case WM_MBUTTONDOWN:
          if (m_mouseKey == VK_MBUTTON) hotkeyDownDetected = true;
          break;

        case WM_MBUTTONUP:
          if (m_mouseKey == VK_MBUTTON) hotkeyUpDetected = true;
          break;
        }
      }

      if (hotkeyDownDetected && !IsCursorOnExcludedProgram(msllHookStruct->pt)) {
        m_mouseDownEventHooked = true;
        return 1;
      }

      if (hotkeyUpDetected && m_mouseDownEventHooked) {
        m_mouseDownEventHooked = false;
        m_wndNotify.PostMessage(m_msgNotify, msllHookStruct->pt.x, msllHookStruct->pt.y);
        return 1;
      }

      m_mouseDownEventHooked = false;
    }
  }

  return CallNextHookEx(m_lowLevelMouseHook, nCode, wParam, lParam);
}

bool MouseGlobalHook::IsCursorOnExcludedProgram(POINT pt) const
{
  if (m_excludedPrograms.size() == 0) return false;

  CWindow window = ::WindowFromPoint(pt);
  if (!window) return false;

  const DWORD dwProcessId = window.GetWindowProcessID();

  DWORD dwDesiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;

  OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
  if (GetVersionEx(&osvi) && osvi.dwMajorVersion <= 5) { dwDesiredAccess = PROCESS_QUERY_INFORMATION; }

  const CHandle process(::OpenProcess(dwDesiredAccess, FALSE, dwProcessId));
  if (!process) return false;

  WCHAR szProcessPath[MAX_PATH];
  if (!GetProcessImageFileName(process, szProcessPath, ARRAYSIZE(szProcessPath))) return false;

  const WCHAR *pProcessName = wcsrchr(szProcessPath, L'\\');
  if (pProcessName)
    pProcessName++;
  else
    pProcessName = szProcessPath;

  for (const auto &program : m_excludedPrograms) {
    if (_wcsicmp(pProcessName, program) == 0) return true;
  }

  return false;
}
