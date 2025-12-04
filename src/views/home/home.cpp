#include "home.h"
#include "../../mouse_hook/mouse_hook.h"
#include "../../utils/utils.h"
#include "config/config.h"

#include <iostream>

Home::Home()
{
    init_ui();
    init_icons();
    init_connect();
}

Home::~Home() = default;

void Home::init_ui()
{
    Borderless       = true;
    IsLayered        = true;
    Opacity          = config::get_or<float>("window/opacity", 1);
    IsHitTestVisible = true;
    StartupLocation  = sw::WindowStartupLocation::CenterScreen;

    Width    = 120;
    Height   = 60;
    MinWidth = 120;
    MaxWidth = config::get_or<int>("window/max-width", 800);
    SetLayout<sw::FillLayout>();

    m_grid.SetRows({sw::AutoSizeGridRow{}, sw::AutoSizeGridRow{}});
    m_grid.SetColumns({sw::FillRemainGridColumn{}});
    m_grid.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
    m_grid.VerticalAlignment   = sw::VerticalAlignment::Stretch;
    m_grid.IsHitTestVisible    = false;
    m_grid.Margin              = sw::Thickness{4, 4, 4, 4};

    m_textBox.HorizontalAlignment = sw::HorizontalAlignment::Stretch;
    m_grid.AddChild(m_textBox, sw::GridLayoutTag{0, 0});

    m_iconBar.Orientation         = sw::Orientation::Horizontal;
    m_iconBar.HorizontalAlignment = sw::HorizontalAlignment::Left;
    m_iconBar.VerticalAlignment   = sw::VerticalAlignment::Center;
    m_iconBar.IsHitTestVisible    = false;
    m_grid.AddChild(m_iconBar, sw::GridLayoutTag{1, 0});
    AddChild(m_grid);
}

void Home::init_icons()
{
    m_iconBoxes.clear();
    m_iconBar.ClearChildren();
}

void Home::init_connect()
{
    MouseHook::instance().install([]() {
        const auto pt   = utils::getMousePosition();
        std::wcout << L"Mouse: " << pt.x << L", " << pt.y << std::endl;
        const auto wStr = utils::getTextByUIA(pt);
        std::wcout << L"Text: " << wStr << std::endl;
    });

    AddHandler<sw::KeyDownEventArgs>([this](sw::UIElement &sender, sw::KeyDownEventArgs &e) {
        if (e.key == sw::VirtualKey::Esc)
        {
            Visible      = false;
            e.handled    = true;
            e.handledMsg = true;
        }
    });
}

void Home::update_text_width()
{
    std::wstring text = m_textBox.Text;
    if (text.empty())
    {
        m_textBox.Width = 60;
        return;
    }

    HWND    hwnd = m_textBox.Handle;
    HDC     hdc  = ::GetDC(hwnd);
    HFONT   hf   = m_textBox.GetFontHandle();
    HGDIOBJ old  = ::SelectObject(hdc, hf);
    RECT    rc{0, 0, 0, 0};
    ::DrawTextW(
        hdc,
        text.c_str(),
        static_cast<int>(text.size()),
        &rc,
        DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX
    );
    ::SelectObject(hdc, old);
    ::ReleaseDC(hwnd, hdc);

    double wDip     = sw::Dip::PxToDipX(rc.right - rc.left);
    double padding  = 12.0;
    double maxDip   = static_cast<double>(MaxWidth);
    double finalW   = std::clamp(wDip + padding, 60.0, maxDip);
    m_textBox.Width = finalW;
}

void Home::populate_icons(const std::vector<std::wstring> &icon_paths, int square_size)
{
    m_iconBar.ClearChildren();
    m_iconBoxes.clear();

    for (const auto &path : icon_paths)
    {
        auto icon         = std::make_unique<sw::IconBox>();
        icon->StretchIcon = true;
        icon->Load(path);

        auto btn                 = std::make_unique<sw::Button>();
        btn->Width               = square_size;
        btn->Height              = square_size;
        btn->HorizontalAlignment = sw::HorizontalAlignment::Left;
        btn->VerticalAlignment   = sw::VerticalAlignment::Center;
        btn->AddChild(*icon);

        m_iconBar.AddChild(*btn);

        m_iconBoxes.emplace_back(std::move(icon));
    }
}

void Home::OnNcHitTest(const sw::Point &testPoint, sw::HitTestResult &result)
{
    sw::Point pt       = this->PointFromScreen(testPoint);
    auto     &cr       = ClientRect;
    bool      inClient = pt.x >= cr->left && pt.y >= cr->top && pt.x < cr->left + cr->width &&
                    pt.y < cr->top + cr->height;
    if (!inClient)
    {
        return;
    }

    const auto &tb     = m_textBox.LogicalRect;
    bool        inText = pt.x >= tb->left && pt.y >= tb->top && pt.x < tb->left + tb->width &&
                  pt.y < tb->top + tb->height;
    if (inText)
    {
        result = sw::HitTestResult::HitClient;
        return;
    }

    result = sw::HitTestResult::HitCaption;
}
