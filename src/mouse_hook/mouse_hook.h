#pragma once

#include <Window.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

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

    // 工作线程相关
    std::thread                       m_worker_thread;
    std::atomic_bool                  m_worker_running{false};
    std::queue<std::function<void()>> m_tasks;
    std::mutex                        m_queue_mutex;
    std::condition_variable           m_queue_cv;

    // 防止重入
    std::atomic_bool m_task_executing{false};

    static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

    bool check_exclude_exe() const;

    void worker_thread_func();
    void post_task(std::function<void()> task);
};