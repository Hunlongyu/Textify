#include "Utils.h"

void utils::getTextFromPointByMSAA(POINT pt, std::wstring &outStr, CRect &outRc)
{
  outStr.clear();
  outRc = CRect{ pt, CSize{ 0, 0 } };

  std::wstring name;
  std::wstring value;
  std::wstring role;

  HWND hwnd = WindowFromPoint(pt);
  if (hwnd == nullptr) { return; }

  IAccessible *pAcc = nullptr;
  VARIANT varChild;

  HRESULT hr = AccessibleObjectFromPoint(pt, &pAcc, &varChild);
  if (hr == S_OK) {
    BSTR bstrName, bstrValue;
    VARIANT varRole;
    VariantInit(&varRole);

    hr = pAcc->get_accName(varChild, &bstrName);// 获取 name
    if (hr == S_OK) { outStr += bstrName; }

    hr = pAcc->get_accValue(varChild, &bstrValue);// 获取 value
    if (hr == S_OK) { outStr += bstrValue; }

    // hr = pAcc->get_accRole(varChild, &varRole);// 获取 role
    // if (hr == S_OK )

    // 对获取的信息进行操作
    SysFreeString(bstrName);
    SysFreeString(bstrValue);
    VariantClear(&varRole);

    pAcc->Release();
  }
}

void utils::getTextFromPointByUIA(POINT pt, std::wstring &outStr, CRect &outRc)
{
  outStr.clear();
  outRc = CRect{ pt, CSize{ 0, 0 } };

  HRESULT hr;
  CComPtr<IUIAutomation> uia;
  hr = uia.CoCreateInstance(CLSID_CUIAutomation);
  if (FAILED(hr) || !uia) { return; }

  CComPtr<IUIAutomationElement> element;
  hr = uia->ElementFromPoint(pt, &element);
  if (FAILED(hr) || !element) { return; }

  CComPtr<IUIAutomationCondition> trueCondition;
  hr = uia->CreateTrueCondition(&trueCondition);
  if (FAILED(hr) || !trueCondition) { return; }

  CComPtr<IUIAutomationTreeWalker> treeWalker;
  hr = uia->CreateTreeWalker(trueCondition, &treeWalker);
  if (FAILED(hr) || !treeWalker) { return; }

  int processId = 0;
  hr = element->get_CurrentProcessId(&processId);
  if (FAILED(hr)) { return; }

  BSTR bsName;
  hr = element->get_CurrentName(&bsName);
  if (SUCCEEDED(hr) && bsName != nullptr) { outStr += bsName; }
  SysFreeString(bsName);

  VARIANT value;
  VariantInit(&value);
  hr = element->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &value);
  if (hr == S_OK && value.vt == VT_BSTR && value.bstrVal != nullptr) { outStr += value.bstrVal; }
  VariantClear(&value);

  CRect b_rect;
  hr = element->get_CurrentBoundingRectangle(&b_rect);
  if (SUCCEEDED(hr)) { outRc = b_rect; }
}
