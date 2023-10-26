#include "Config.h"
#include "../../include/nlohmann/json.hpp"

#include <codecvt>
#include <iostream>

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
  m_HotKey.left = false;
  m_HotKey.mid = jsonObj["hotkey"].value("mid", false);
  m_HotKey.right = jsonObj["hotkey"].value("right", true);

  btn_lists.clear();
  const auto lists = jsonObj["list"];
  for (auto item : lists) {
    Btn btn;
    const auto type = item.at("type").get<std::string>();
    btn.type = std::wstring(type.begin(), type.end());
    const auto tips = item.at("tips").get<std::string>();
    btn.tips = std::wstring(tips.begin(), tips.end());
    const auto icon = item.at("icon").get<std::string>();
    btn.icon = std::wstring(icon.begin(), icon.end());
    if (item.contains("command")) {
      const auto command = item.at("command").get<std::string>();
      btn.command = std::wstring(command.begin(), command.end());
    }
    if (item.contains("app")) { btn.app = item.at("app").get<bool>(); }
    btn_lists.push_back(btn);
  }

  return true;
}

bool Config::save() { return true; }

// 递归查找 config.json 配置文件
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