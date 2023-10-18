#include "config.h"
#include "../../third_party/json/single_include/nlohmann/json.hpp"
#include "fstream"

using nlohmann::json;

Config::Config() {}
Config::~Config() {}

bool Config::load_json_config()
{
  const auto configPath = find_config_file();
  if (configPath.empty()) { return false; }

  std::ifstream jsonFile(configPath);
  json jsonObj;
  jsonFile >> jsonObj;
  jsonFile.close();

  m_HotKey.enable = jsonObj["hotkey"].value("enable", false);
  m_HotKey.shift = jsonObj["hotkey"].value("shift", false);
  m_HotKey.alt = jsonObj["hotkey"].value("alt", false);
  m_HotKey.ctrl = jsonObj["hotkey"].value("ctrl", true);
  m_HotKey.left = jsonObj["hotkey"].value("left", false);
  m_HotKey.mid = jsonObj["hotkey"].value("mid", false);
  m_HotKey.right = jsonObj["hotkey"].value("right", true);

  return true;
}

bool Config::save_json_config() { return true; }

// 递归查找 config.json 配置文件
fs::path Config::find_config_file()
{
  const fs::path root = ".";
  try {
    for (auto &p : fs::recursive_directory_iterator(root)) {
      if (p.path().filename() == "config.json") { return p.path(); }
    }
  } catch (...) {}
  return {};
}
