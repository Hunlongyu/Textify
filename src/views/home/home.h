#pragma once

#include "SimpleWindow.h"

class Home : public sw::Window
{
  public:
    Home();
    ~Home() override;

  protected:
    void OnNcHitTest(const sw::Point &testPoint, sw::HitTestResult &result) override;
    bool OnKillFocus(HWND hNextFocus) override;

  private:
    sw::Grid                                  m_grid;
    sw::StackPanel                            m_iconBar;
    sw::TextBox                               m_textBox;
    std::vector<std::unique_ptr<sw::IconBox>> m_iconBoxes;

    struct IconItem
    {
        std::wstring icon_path;
        std::string  command;
        std::string  type;
        std::string  tips;
        bool         hide = false;
    };

    void init_ui();

    void init_icons();

    void init_connect();

    void enable_window_shadow() const;

    void update_text_width();

    void populate_icons(const std::vector<IconItem> &items, int square_size = 20);
};
