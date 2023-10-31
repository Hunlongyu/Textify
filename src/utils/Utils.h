#pragma once
#include <Windows.h>
#include <atlcomcli.h>
#include <regex>
#include <string>
#include <uiautomationclient.h>
#include <vector>

namespace utils {
// 使用微软 UIA 技术获取文字
void getTextFromPointByUIA(POINT pt, std::wstring &outStr, std::vector<size_t> &outLengths);

// 判断 2 个 BSTR 是否相等
bool AreBSTREqual(const BSTR &b1, const BSTR &b2);

// 处理文字
void processingText(std::wstring &txt, const std::wstring &str, std::vector<size_t> &lengthList);

}// namespace utils
