#pragma once

#include <optional>
#include <string>
#include <wrl/client.h>

// 前向声明
struct IUIAutomation;
struct IUIAutomationElement;
struct IAccessible;
struct tagVARIANT;
using VARIANT = tagVARIANT;
struct tagPOINT;
using POINT = tagPOINT;

class Utils final
{
  public:
    // 删除拷贝和移动
    Utils(const Utils &)            = delete;
    Utils &operator=(const Utils &) = delete;
    Utils(Utils &&)                 = delete;
    Utils &operator=(Utils &&)      = delete;

    // 获取单例实例
    static Utils &getInstance() noexcept;

    // 使用 UIA 获取鼠标位置的文本
    std::wstring getTextByUIA();

    // 使用 MSAA 获取鼠标位置的文本
    std::wstring getTextByMSAA();

  private:
    Utils()  = default;
    ~Utils() = default;

    // UIA 实现
    std::optional<std::wstring> getAccessibleInfoFromPointUIA(POINT pt);

    // MSAA 实现
    std::optional<std::wstring> getAccessibleInfoFromPointMSAA(POINT pt);

    // 从 UIA 元素提取文本
    std::wstring extractTextFromUIAElement(IUIAutomationElement *element);

    // 从 MSAA 元素提取文本
    std::wstring extractTextFromMSAAElement(IAccessible *acc, const VARIANT &childId);
};