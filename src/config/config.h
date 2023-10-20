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

  HotKey m_HotKey;
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
