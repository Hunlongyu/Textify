#pragma once
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
    bool app{ false };
  };


  HotKey m_HotKey;
  std::vector<Btn> btn_lists;
  static Config &Instance();

  bool load();
  bool save();

private:
  static Config *instance_;
  static std::mutex mutex_;

  Config();
  ~Config();

  Config(const Config &);
  Config &operator=(const Config &);

  fs::path find_config_filepath();
};
