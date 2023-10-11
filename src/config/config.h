#pragma once
#include <filesystem>
namespace fs = std::filesystem;

struct HotKey
{
  bool ctrl;
  bool alt;
  bool shift;
  bool left;
  bool mid;
  bool right;
};

class Config
{
public:
  static Config &getInstance()
  {
    static Config instance;
    return instance;
  }

  bool load_json_config();
  static bool save_json_config();

  HotKey m_HotKey;

  bool m_checkForUpdate;
  bool m_autoCopy;
  bool m_autoStartup;
  bool m_admin;

private:
  Config();
  ~Config();

  void operator=(Config const &) = delete;

  static fs::path find_config_file();
};
