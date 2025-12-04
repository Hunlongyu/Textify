#include "mouse_hook.h"

#include "Utils.h"
#include "config/config.h"

std::atomic<bool> MouseHook::s_pressed{false};

MouseHook::MouseHook()
{
    refresh_config();
}

MouseHook::~MouseHook()
{
    uninstall();
}

MouseHook &MouseHook::instance()
{
    static MouseHook instance;
    return instance;
}

bool MouseHook::install(const std::function<void()> &callback)
{
    if (m_hook)
    {
        return true;
    }

    m_callback = callback;
    if (!m_config.load(std::memory_order_acquire)->enabled)
    {
        return false;
    }

    m_hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
    return m_hook != nullptr;
}

void MouseHook::uninstall()
{
    if (m_hook)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
    m_callback = nullptr;
}

void MouseHook::refresh_config()
{
    const auto new_cfg = std::make_shared<Config>();
    new_cfg->enabled   = config::get_or<bool>("hotkey/enabled", true);
    new_cfg->left      = config::get_or<bool>("hotkey/left", false);
    new_cfg->mid       = config::get_or<bool>("hotkey/mid", false);
    new_cfg->right     = config::get_or<bool>("hotkey/right", true);
    new_cfg->alt       = config::get_or<bool>("hotkey/alt", true);
    new_cfg->ctrl      = config::get_or<bool>("hotkey/ctrl", false);
    new_cfg->shift     = config::get_or<bool>("hotkey/shift", false);

    const auto list = config::get_or<std::vector<std::string>>("exclude", {"Notepad3.exe"});
    std::vector<std::wstring> exclude_list;
    for (auto &excluded : list)
    {
        std::string lowStr;
        lowStr.resize(excluded.size());
        std::ranges::transform(excluded, lowStr.begin(), ::towlower);
        std::wstring ws = sw::Utils::ToWideStr(lowStr);
        exclude_list.push_back(ws);
    }
    new_cfg->exclude_exes = exclude_list;

    m_config.store(new_cfg, std::memory_order_release);
}

void MouseHook::set_enabled(const bool enabled) const noexcept
{
    if (const auto cfg = m_config.load(std::memory_order_relaxed))
    {
        cfg->enabled = enabled;
    }
}

LRESULT CALLBACK
MouseHook::LowLevelMouseProc(const int nCode, const WPARAM wParam, const LPARAM lParam)
{
    if (nCode != HC_ACTION)
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto *info = reinterpret_cast<const MSLLHOOKSTRUCT *>(lParam);
    if (info->flags & (LLMHF_INJECTED | LLMHF_LOWER_IL_INJECTED))
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto cfg = instance().m_config.load(std::memory_order_acquire);
    if (!cfg || !cfg->enabled)
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const bool isDown = (wParam == WM_LBUTTONDOWN && cfg->left) ||
                        (wParam == WM_MBUTTONDOWN && cfg->mid) ||
                        (wParam == WM_RBUTTONDOWN && cfg->right);

    const bool isUp = (wParam == WM_LBUTTONUP && cfg->left) ||
                      (wParam == WM_MBUTTONUP && cfg->mid) ||
                      (wParam == WM_RBUTTONUP && cfg->right);

    if (!isDown && !isUp)
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (cfg->alt && !(GetKeyState(VK_MENU) & 0x8000))
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    if (cfg->ctrl && !(GetKeyState(VK_CONTROL) & 0x8000))
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    if (cfg->shift && !(GetKeyState(VK_SHIFT) & 0x8000))
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (instance().check_exclude_exe())
    {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (isDown)
    {
        s_pressed.store(true, std::memory_order_relaxed);
        return 1;
    }

    if (isUp && s_pressed.exchange(false, std::memory_order_relaxed))
    {
        if (instance().m_callback)
        {
            instance().m_callback();
        }
        return 1;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool MouseHook::check_exclude_exe() const
{
    const auto cfg = m_config.load(std::memory_order_acquire);
    if (!cfg || cfg->exclude_exes.empty())
    {
        return false;
    }

    POINT pt;
    if (!GetCursorPos(&pt))
    {
        return false;
    }

    const HWND hwnd = WindowFromPoint(pt);
    if (!hwnd)
    {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0)
    {
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess)
    {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (!hProcess)
        {
            return false;
        }
    }

    wchar_t    path[MAX_PATH] = {};
    DWORD      size           = _countof(path);
    const bool ok             = QueryFullProcessImageNameW(hProcess, 0, path, &size);
    CloseHandle(hProcess);
    if (!ok)
    {
        return false;
    }

    std::wstring filename = std::filesystem::path(path).filename();
    std::wstring lowered;
    lowered.resize(filename.size());
    std::ranges::transform(filename, lowered.begin(), ::towlower);

    for (const auto &excluded : cfg->exclude_exes)
    {
        if (lowered.find(excluded) != std::wstring::npos)
        {
            return true;
        }
    }

    return false;
}
