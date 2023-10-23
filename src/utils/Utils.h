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

// 处理文字
void processingText(std::wstring &txt, const std::wstring &str, std::vector<size_t> &lengthList);

}// namespace utils
