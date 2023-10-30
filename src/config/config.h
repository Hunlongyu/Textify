#pragma once
#include "../config/CWin.h"
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <mutex>

namespace fs = std::filesystem;

class Config
{
public:
  struct HotKey
  {
    bool enable;
    bool ctrl;
    bool alt;
    bool shift;
    bool left;
    bool mid;
    bool right;
  };

  struct Btn
  {
    std::wstring type;
    std::wstring tips;
    std::wstring icon;
    std::wstring command;
    bool hide{ false };
  };

  struct ConfigData
  {
    bool checkForUpdate{ false };
    bool autoStartup{ false };
    bool admin{ false };
    int maxWidth{ 800 };
    HotKey hotkey;
    std::vector<Btn> list;
  };

  ConfigData m_config;

  static Config &Instance();

  bool init();
  bool load();
  bool save();

private:
  static Config *instance_;
  static std::mutex mutex_;

  Config();
  ~Config();

  Config(const Config &);
  Config &operator=(const Config &);

  static fs::path find_config_filepath();

  // 检查配置是否生效
  bool checkConfig() const;

  // 检查软件是否以管理员权限启动
  static bool isRunAsAdmin();

  // 尝试管理员权限启动
  static bool ElevateNow();
};
