#pragma once

struct HotKey
{
  int key;
  bool ctrl;
  bool alt;
  bool shift;
};

class Config
{
public:
  Config();
  ~Config();

  bool loadJsonConfig();
  bool saveJsonConfig();

  HotKey m_mouseHotKey;
  HotKey m_keybdHotKey;

  bool m_checkForUpdate;
  bool m_autoCopy;
  bool m_autoStartup;
  bool m_admin;

};
