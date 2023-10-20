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

void utils::getTextFromPointByUIA(POINT pt, std::wstring &outStr, CRect &outRc) {}
