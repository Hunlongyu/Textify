#pragma once

#include <SimpleWindow.h>

class Home : public sw::Window
{
  public:
    Home();
    ~Home() override;

  protected:
    void OnNcHitTest(const sw::Point &testPoint, sw::HitTestResult &result) override;

  private:
    sw::Grid                                  m_grid;
    sw::StackPanel                            m_iconBar;
    sw::TextBox                               m_textBox;
    std::vector<std::unique_ptr<sw::IconBox>> m_iconBoxes;

    void init_ui();

    void init_icons();

    void init_connect();

    void update_text_width();
    void populate_icons(const std::vector<std::wstring> &icon_paths, int square_size = 48);
};
