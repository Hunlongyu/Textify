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

  element.Release();
  hr = uia->ElementFromPoint(pt, &element);
  if (FAILED(hr) || !element) { return; }

  CComPtr<IUIAutomationCondition> trueCondition;
  hr = uia->CreateTrueCondition(&trueCondition);
  if (FAILED(hr) || !trueCondition) return;

  CComPtr<IUIAutomationTreeWalker> treeWalker;
  hr = uia->CreateTreeWalker(trueCondition, &treeWalker);
  if (FAILED(hr) || !treeWalker) return;

  int processId = 0;
  hr = element->get_CurrentProcessId(&processId);
  if (FAILED(hr)) return;

  constexpr int maxDepth{ 5 };
  int depth{ 0 };
  while (true) {
    if (depth > maxDepth) { break; }
    std::wstring txt;
    std::vector<size_t> lengths;

    BSTR bsName;
    hr = element->get_CurrentName(&bsName);
    if (SUCCEEDED(hr) && bsName != nullptr) { processingText(txt, bsName, lengths); }

    VARIANT value;
    hr = element->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &value);
    if (SUCCEEDED(hr) && value.vt == VT_BSTR && value.bstrVal && value.bstrVal != bsName) {
      processingText(txt, value.bstrVal, lengths);
    }

    SysFreeString(bsName);
    VariantClear(&value);

    if (!txt.empty()) {
      outStr = txt;
      outLengths = lengths;
      break;
    }

    CComPtr<IUIAutomationElement> parentElement;
    hr = treeWalker->GetParentElement(element, &parentElement);
    if (FAILED(hr) || !parentElement) break;

    int compareProcessId = 0;
    hr = parentElement->get_CurrentProcessId(&compareProcessId);
    if (FAILED(hr) || compareProcessId != processId) break;

    depth++;
    element.Attach(parentElement.Detach());
  }
}

void utils::processingText(std::wstring &txt, const std::wstring &str, std::vector<size_t> &lengthList)
{
  if (str.empty()) return;

  std::wstring append = str;
  std::wstring::size_type pos = 0;
  while ((pos = append.find(L"\r\n", pos)) != std::wstring::npos) {
    append.replace(pos, 2, L" ");
    pos += 1;
  }
  std::ranges::replace(append, L'\r', L' ');
  std::ranges::replace(append, L'\n', L' ');

  if (!txt.empty()) {
    txt += L"\r\n";
    lengthList.push_back(txt.length());
  }

  txt += append;
}
