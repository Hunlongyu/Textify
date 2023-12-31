﻿#include "Config.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>
#include <sddl.h>

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

bool Config::init()
{
  auto isOk{ false };
  isOk = load() && checkConfig();
  return isOk;
}

bool Config::load()
{
  const auto configPath = find_config_filepath();
  if (configPath.empty()) { return false; }

  configJsonPath_ = configPath;

  json jsonObj;
  std::ifstream jsonFile(configPath);
  try {
    jsonFile >> jsonObj;
    jsonFile.close();
  } catch (...) {
    MessageBox(nullptr, L"配置文件解析失败，请检查文件格式后重试。", L"错误", NULL);
    jsonFile.close();
    return false;
  }

  try {
    m_config.hotkey.enable = jsonObj["hotkey"].value("enable", false);
    m_config.hotkey.shift = jsonObj["hotkey"].value("shift", false);
    m_config.hotkey.alt = jsonObj["hotkey"].value("alt", false);
    m_config.hotkey.ctrl = jsonObj["hotkey"].value("ctrl", true);
    m_config.hotkey.left = false;
    m_config.hotkey.mid = jsonObj["hotkey"].value("mid", false);
    m_config.hotkey.right = jsonObj["hotkey"].value("right", true);

    m_config.list.clear();
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
      if (item.contains("hide")) { btn.hide = item.at("hide").get<bool>(); }
      m_config.list.push_back(btn);
    }

    m_config.exclude.clear();
    const auto excludes = jsonObj["exclude"];
    for (const auto &item : excludes) {
      const auto exclude = item.get<std::string>();
      m_config.exclude.emplace_back(exclude.begin(), exclude.end());
    }

    m_config.checkForUpdate = jsonObj.value("checkForUpdate", false);
    m_config.autoStartup = jsonObj.value("autoStartup", false);
    m_config.admin = jsonObj.value("admin", false);
    m_config.maxWidth = jsonObj.value("max-width", 800);
  } catch (...) {
    MessageBox(nullptr, L"配置文件解析失败，请检查文件格式后重试。", L"错误", NULL);
    return false;
  }
  return true;
}

bool Config::save()
{
  json jsonObj;
  jsonObj["hotkey"]["enable"] = m_config.hotkey.enable;
  jsonObj["hotkey"]["shift"] = m_config.hotkey.shift;
  jsonObj["hotkey"]["alt"] = m_config.hotkey.alt;
  jsonObj["hotkey"]["ctrl"] = m_config.hotkey.ctrl;
  jsonObj["hotkey"]["left"] = m_config.hotkey.left;
  jsonObj["hotkey"]["mid"] = m_config.hotkey.mid;
  jsonObj["hotkey"]["right"] = m_config.hotkey.right;

  jsonObj["list"].clear();
  for (const auto &item : m_config.list) {
    json btn;
    btn["type"] = std::string(item.type.begin(), item.type.end());
    btn["tips"] = std::string(item.tips.begin(), item.tips.end());
    btn["icon"] = std::string(item.icon.begin(), item.icon.end());
    btn["hide"] = item.hide;
    if (!item.command.empty()) { btn["command"] = std::string(item.command.begin(), item.command.end()); }
    jsonObj["list"].push_back(btn);
  }

  jsonObj["exclude"].clear();
  for (const auto &item : m_config.exclude) { jsonObj["exclude"].push_back(std::string(item.begin(), item.end())); }

  jsonObj["admin"] = m_config.admin;
  jsonObj["autoStartup"] = m_config.autoStartup;
  jsonObj["checkForUpdate"] = m_config.checkForUpdate;
  jsonObj["max-width"] = m_config.maxWidth;

  std::ofstream jsonFile(configJsonPath_);
  jsonFile << jsonObj.dump(2) << std::endl;
  jsonFile.close();
  return true;
}

bool Config::change(const std::string &key, const bool &value)
{
  if (key == "admin") {
    m_config.admin = value;
    return true;
  }
  if (key == "hotkey.shift") {
    m_config.hotkey.shift = value;
    return true;
  }
  if (key == "hotkey.ctrl") {
    m_config.hotkey.ctrl = value;
    return true;
  }
  if (key == "hotkey.alt") {
    m_config.hotkey.alt = value;
    return true;
  }
  if (key == "hotkey.mid") {
    m_config.hotkey.mid = value;
    return true;
  }
  if (key == "hotkey.right") {
    m_config.hotkey.right = value;
    return true;
  }
  return false;
}

// 检查配置文件，并使其生效
bool Config::checkConfig() const
{
  bool isOk{ true };
  if (m_config.admin) { isOk = ElevateNow(); }
  return isOk;
}

// 尝试管理员权限启动
bool Config::ElevateNow()
{
  bool isAdmin = false;
  try { isAdmin = isRunAsAdmin(); } catch (...) {
    MessageBox(nullptr, L"", L"", NULL);
    return false;
  }

  if (!isAdmin) {
    WCHAR szPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath))) {
      SHELLEXECUTEINFO sei = { sizeof(sei) };
      sei.lpVerb = L"runas";// 操作，runas表示以管理员权限运行
      sei.lpFile = szPath;// 要运行的程序
      sei.hwnd = nullptr;
      sei.nShow = SW_NORMAL;

      if (!ShellExecuteEx(&sei)) {
        MessageBox(nullptr, L"软件提升管理员权限失败，请右键属性自行勾选管理员权限。", L"错误", NULL);
        return false;
      }
      exit(0);
    }
  }
  return true;
}

// 判断是否是管理员权限启动
bool Config::isRunAsAdmin()
{
  bool isRunAsAdmin = false;
  HANDLE hToken = nullptr;
  // 打开当前进程的访问令牌
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
    // 检索指定的访问令牌信息
    TOKEN_ELEVATION elevation;
    DWORD dwSize;
    if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
      // TokenElevation 表示此进程是否处于提升的状态，TokenIsElevated的值为1表示已提升
      isRunAsAdmin = elevation.TokenIsElevated;
    }
  }
  if (hToken) { CloseHandle(hToken); }
  return isRunAsAdmin;
}

// 递归查找 config.json 配置文件
fs::path Config::find_config_filepath()
{
  fs::path root;
#ifdef _DEBUG
  root = ".";
#else
  root = std::filesystem::current_path();
#endif
  try {
    for (auto &p : fs::recursive_directory_iterator(root)) {
      if (p.path().filename() == "config.json") { return p.path(); }
    }
  } catch (...) {}
  return {};
}
