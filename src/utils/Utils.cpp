#include "Utils.h"
#include <UIAutomation.h>
#include <oleacc.h>
#include <atlbase.h>
#include <Windows.h>

using Microsoft::WRL::ComPtr;

Utils& Utils::getInstance() noexcept
{
    static Utils instance;
    return instance;
}

std::wstring Utils::getTextByUIA()
{
    POINT pt;
    if (!::GetCursorPos(&pt))
        return {};

    auto result = getAccessibleInfoFromPointUIA(pt);
    return result.value_or(L"");
}

std::wstring Utils::getTextByMSAA()
{
    POINT pt;
    if (!::GetCursorPos(&pt))
        return {};

    auto result = getAccessibleInfoFromPointMSAA(pt);
    return result.value_or(L"");
}

std::optional<std::wstring> Utils::getAccessibleInfoFromPointUIA(POINT pt)
{
    ComPtr<IUIAutomation> uia;
    HRESULT hr = ::CoCreateInstance(
        CLSID_CUIAutomation,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&uia)
    );
    
    if (FAILED(hr) || !uia)
        return std::nullopt;

    ComPtr<IUIAutomationElement> element;
    hr = uia->ElementFromPoint(pt, &element);
    if (FAILED(hr) || !element)
        return L"";

    // Chromium 的 bug 修复：需要第二次查询
    element.Reset();
    hr = uia->ElementFromPoint(pt, &element);
    if (FAILED(hr) || !element)
        return L"";

    ComPtr<IUIAutomationCondition> trueCondition;
    hr = uia->CreateTrueCondition(&trueCondition);
    if (FAILED(hr) || !trueCondition)
        return L"";

    ComPtr<IUIAutomationTreeWalker> treeWalker;
    hr = uia->CreateTreeWalker(trueCondition.Get(), &treeWalker);
    if (FAILED(hr) || !treeWalker)
        return L"";

    int processId = 0;
    hr = element->get_CurrentProcessId(&processId);
    if (FAILED(hr))
        return L"";

    // 遍历元素树查找文本
    int depth = 0;
    const int MAX_DEPTH = 10;
    
    while (element && depth < MAX_DEPTH)
    {
        std::wstring text = extractTextFromUIAElement(element.Get());
        if (!text.empty())
            return text;

        ComPtr<IUIAutomationElement> parentElement;
        hr = treeWalker->GetParentElement(element.Get(), &parentElement);
        if (FAILED(hr) || !parentElement)
            break;

        int compareProcessId = 0;
        hr = parentElement->get_CurrentProcessId(&compareProcessId);
        if (FAILED(hr) || compareProcessId != processId)
            break;

        element = std::move(parentElement);
        depth++;
    }

    return L"";
}

std::optional<std::wstring> Utils::getAccessibleInfoFromPointMSAA(POINT pt)
{
    ComPtr<IAccessible> acc;
    CComVariant childId;
    
    HRESULT hr = ::AccessibleObjectFromPoint(pt, &acc, &childId);
    if (FAILED(hr) || !acc)
        return std::nullopt;

    // Chromium 的 bug 修复：需要第二次查询
    acc.Reset();
    childId.Clear();
    hr = ::AccessibleObjectFromPoint(pt, &acc, &childId);
    if (FAILED(hr) || !acc)
        return std::nullopt;

    HWND hWnd = nullptr;
    hr = ::WindowFromAccessibleObject(acc.Get(), &hWnd);
    if (FAILED(hr))
        return L"";

    DWORD processId = 0;
    ::GetWindowThreadProcessId(hWnd, &processId);

    // 遍历可访问对象树查找文本
    int depth = 0;
    const int MAX_DEPTH = 10;
    
    while (acc && depth < MAX_DEPTH)
    {
        std::wstring text = extractTextFromMSAAElement(acc.Get(), childId);
        if (!text.empty())
            return text;

        if (childId.lVal == CHILDID_SELF)
        {
            ComPtr<IDispatch> dispParent;
            hr = acc->get_accParent(&dispParent);
            if (FAILED(hr) || !dispParent)
                break;

            ComPtr<IAccessible> accParent;
            hr = dispParent.As(&accParent);
            if (FAILED(hr))
                break;

            HWND parentHwnd = nullptr;
            hr = ::WindowFromAccessibleObject(accParent.Get(), &parentHwnd);
            if (FAILED(hr))
                break;

            DWORD compareProcessId = 0;
            ::GetWindowThreadProcessId(parentHwnd, &compareProcessId);
            if (compareProcessId != processId)
                break;

            acc = std::move(accParent);
        }
        else
        {
            childId.lVal = CHILDID_SELF;
        }
        depth++;
    }

    return L"";
}

std::wstring Utils::extractTextFromUIAElement(IUIAutomationElement* element)
{
    if (!element)
        return {};

    std::wstring result;

    // 获取名称
    CComBSTR name;
    if (SUCCEEDED(element->get_CurrentName(&name)) && name)
    {
        result += name.m_str;
    }

    // 获取值
    CComVariant value;
    if (SUCCEEDED(element->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &value)) &&
        value.vt == VT_BSTR && value.bstrVal)
    {
        if (name != value.bstrVal)
        {
            if (!result.empty())
                result += L" ";
            result += value.bstrVal;
        }
    }

    return result;
}

std::wstring Utils::extractTextFromMSAAElement(IAccessible* acc, const VARIANT& childId)
{
    if (!acc)
        return {};

    std::wstring result;

    // 获取名称
    CComBSTR name;
    if (SUCCEEDED(acc->get_accName(childId, &name)) && name)
    {
        result += name.m_str;
    }

    // 获取值
    CComBSTR value;
    if (SUCCEEDED(acc->get_accValue(childId, &value)) && value && value != name)
    {
        if (!result.empty())
            result += L" ";
        result += value.m_str;
    }

    // 获取描述(排除标题栏)
    CComVariant role;
    if (FAILED(acc->get_accRole(CComVariant(CHILDID_SELF), &role)) ||
        role.lVal != ROLE_SYSTEM_TITLEBAR)
    {
        CComBSTR description;
        if (SUCCEEDED(acc->get_accDescription(childId, &description)) &&
            description && description != name && description != value)
        {
            if (!result.empty())
                result += L" ";
            result += description.m_str;
        }
    }

    return result;
}