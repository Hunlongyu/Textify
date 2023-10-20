#include "Config.h"
#include "../../include/nlohmann/json.hpp"

using nlohmann::json;

Config *Config::instance_ = nullptr;
std::mutex Config::mutex_;


Config::Config() {}
Config::~Config() {}

Config &Config::Instance()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (instance_ == nullptr) { instance_ = new Config(); }
  return *instance_;
}

bool Config::load()
{
  const auto configPath = find_config_filepath();
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

bool Config::save() { return true; }

// µ›πÈ≤È’“ config.json ≈‰÷√Œƒº˛
fs::path Config::find_config_filepath()
{
  const fs::path root = ".";
  try {
    for (auto &p : fs::recursive_directory_iterator(root)) {
      if (p.path().filename() == "config.json") { return p.path(); }
    }
  } catch (...) {}
  return {};
}