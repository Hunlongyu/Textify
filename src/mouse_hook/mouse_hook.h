#pragma once

#include <Window.h>
#include <functional>

class MouseHook
{
  public:
    MouseHook(const MouseHook &)            = delete;
    MouseHook &operator=(const MouseHook &) = delete;

    static MouseHook &instance();

    bool install(const std::function<void()> &callback);

    void uninstall();

    void refresh_config();

    bool is_installed() const noexcept { return m_hook != nullptr; }

    void set_enabled(bool enabled) const noexcept;

  private:
    MouseHook();
    ~MouseHook();

    struct Config
    {
        bool                      enabled = true;
        bool                      left    = false;
        bool                      mid     = false;
        bool                      right   = true;
        bool                      alt     = true;
        bool                      ctrl    = false;
        bool                      shift   = false;
        std::vector<std::wstring> exclude_exes;
    };

    HHOOK                 m_hook = nullptr;
    std::function<void()> m_callback;

    std::atomic<std::shared_ptr<Config>> m_config;

    static std::atomic_bool s_pressed;

    static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

    bool check_exclude_exe() const;
};
