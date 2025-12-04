#include "utils.h"

#include <UIAutomation.h>
#include <memory>
#include <oleacc.h>
#include <windows.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace utils
{
POINT getMousePosition()
{
    POINT pt;
    if (!GetCursorPos(&pt))
    {
        return {0, 0};
    }

    // 方式A：优先使用窗口 DPI（最准确，Win10 1607+）
    const HWND hwnd = WindowFromPoint(pt);
    if (hwnd)
    {
        UINT dpi = GetDpiForWindow(hwnd); // 需要 manifest 设置 dpiAware=true/pm
        if (dpi != 0 && dpi != USER_DEFAULT_SCREEN_DPI)
        {
            pt.x = MulDiv(pt.x, dpi, USER_DEFAULT_SCREEN_DPI);
            pt.y = MulDiv(pt.y, dpi, USER_DEFAULT_SCREEN_DPI);
        }
    }
    else
    {
        // 方式B：降级使用系统 DPI（老系统兼容）
        UINT dpi = GetDpiForSystem();
        if (dpi != 0 && dpi != USER_DEFAULT_SCREEN_DPI)
        {
            pt.x = MulDiv(pt.x, dpi, USER_DEFAULT_SCREEN_DPI);
            pt.y = MulDiv(pt.y, dpi, USER_DEFAULT_SCREEN_DPI);
        }
    }
    return {pt.x, pt.y};
}

std::wstring getTextByUIA(const POINT pos)
{
    // 1. 初始化 COM（使用 RAII 自动 CoUninitialize）
    auto com_guard =
        std::unique_ptr<void, void (*)(void *)>(reinterpret_cast<void *>(1), [](void *) {
            CoUninitialize();
        });

    // 2. 创建 UIA 主接口
    ComPtr<IUIAutomation> automation;
    HRESULT               hr = CoCreateInstance(
        __uuidof(CUIAutomation8),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IUIAutomation),
        (void **)&automation
    );
    if (FAILED(hr) || !automation)
        return {};

    // 3. 直接从鼠标位置获取元素（最快）
    ComPtr<IUIAutomationElement> element;
    hr = automation->ElementFromPoint(pos, &element);
    if (FAILED(hr) || !element)
        return {};

    // 4. 优雅提取文本的 lambda（支持多种 Pattern）
    const auto tryGetText = [&](auto getTextFunc) -> std::wstring {
        BSTR bStr = nullptr;
        if (SUCCEEDED(getTextFunc(&bStr)) && bStr && SysStringLen(bStr) > 0)
        {
            std::wstring result(bStr, SysStringLen(bStr));
            SysFreeString(bStr);
            return result;
        }
        if (bStr)
            SysFreeString(bStr);
        return {};
    };

    // 5. 按优先级尝试多种方式获取文本

    // 方式1：Name 属性（最常见）
    std::wstring text = tryGetText([&](BSTR *p) {
        return element->get_CurrentName(p);
    });
    if (!text.empty())
    {
        return text;
    }

    // 方式2：ValuePattern（输入框）
    {
        ComPtr<IUIAutomationValuePattern> valuePattern;
        if (SUCCEEDED(element->GetCurrentPatternAs(
                UIA_ValuePatternId, __uuidof(IUIAutomationValuePattern), (void **)&valuePattern
            )) &&
            valuePattern)
        {
            text = tryGetText([&](BSTR *p) {
                return valuePattern->get_CurrentValue(p);
            });
            if (!text.empty())
                return text;
        }
    }

    // 方式3：TextPattern（富文本）
    {
        ComPtr<IUIAutomationTextPattern> textPattern;
        if (SUCCEEDED(element->GetCurrentPatternAs(
                UIA_TextPatternId, __uuidof(IUIAutomationTextPattern), (void **)&textPattern
            )) &&
            textPattern)
        {

            ComPtr<IUIAutomationTextRange> range;
            if (SUCCEEDED(textPattern->get_DocumentRange(&range)) && range)
            {
                text = tryGetText([&](BSTR *p) {
                    return range->GetText(-1, p);
                });
                if (!text.empty())
                    return text;
            }
        }
    }

    // 方式4：LegacyIAccessiblePattern（兼容古老控件）
    {
        ComPtr<IUIAutomationLegacyIAccessiblePattern> legacy;
        if (SUCCEEDED(element->GetCurrentPatternAs(
                UIA_LegacyIAccessiblePatternId,
                __uuidof(IUIAutomationLegacyIAccessiblePattern),
                (void **)&legacy
            )) &&
            legacy)
        {
            text = tryGetText([&](BSTR *p) {
                return legacy->get_CurrentName(p);
            });
            if (!text.empty())
            {
                return text;
            }
        }
    }

    return {}; // 全部失败
}

std::wstring getTextByMSAA(POINT pos)
{
    return {};
}
} // namespace utils