#include "Utils.h"

void utils::getTextFromPointByUIA(POINT pt, std::wstring &outStr, std::vector<size_t> &outLengths)
{
  outStr.clear();
  outLengths = std::vector<size_t>();

  HRESULT hr;
  CComPtr<IUIAutomation> uia;
  hr = uia.CoCreateInstance(CLSID_CUIAutomation);
  if (FAILED(hr) || !uia) { return; }

  CComPtr<IUIAutomationElement> element;
  hr = uia->ElementFromPoint(pt, &element);
  if (FAILED(hr) || !element) { return; }

  std::wstring txt;
  std::vector<size_t> lengths;
  BSTR bsName;
  hr = element->get_CurrentName(&bsName);
  if (SUCCEEDED(hr) && bsName != nullptr) { processingText(txt, bsName, lengths); }
  SysFreeString(bsName);

  VARIANT value;
  hr = element->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &value);
  if (SUCCEEDED(hr) && value.vt == VT_BSTR && value.bstrVal && value.bstrVal != bsName) {
    processingText(txt, value.bstrVal, lengths);
  }
  VariantClear(&value);

  if (!txt.empty()) {
    outStr = txt;
    outLengths = lengths;
  }
}

void utils::processingText(std::wstring &txt, const std::wstring &str, std::vector<size_t> &lengthList)
{
  std::wstring append = str;
  // Convert all newlines to CRLF and trim trailing newlines.
  std::wstring::size_type n = 0;
  while ((n = append.find(L"\r\n")) != std::wstring::npos) { append.replace(n, 2, L"\n"); }
  while ((n = append.find(L"\r")) != std::wstring::npos) { append.replace(n, 1, L"\n"); }

  // Trim trailing newlines.
  const auto pos = append.find_last_not_of(L"\n");
  if (pos != std::wstring::npos) {
    append.erase(pos + 1);
  } else {
    append.clear();// string is all newlines or empty
  }

  while ((n = append.find(L"\n")) != std::wstring::npos) { append.replace(n, 1, L"\r\n"); }

  if (append.empty()) return;

  if (!txt.empty()) {
    txt += L"\r\n";
    lengthList.push_back(txt.length());
  }

  txt += append;
}
