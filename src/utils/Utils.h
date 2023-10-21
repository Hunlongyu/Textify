#pragma once
#include <Windows.h>
#include <atlcomcli.h>
#include <atltypes.h>
#include <oleacc.h>
#include <psapi.h>
#include <string>
#include <uiautomationclient.h>
#include <vector>

namespace utils {

void getTextFromPointByMSAA(POINT pt, std::wstring &outStr, CRect &outRc);

// 使用微软 UIA 技术获取文字
void getTextFromPointByUIA(POINT pt, std::wstring &outStr, CRect &outRc, std::vector<size_t> &outLengths);

// 处理文字
void processingText(std::wstring &txt, const std::wstring &str, std::vector<size_t> &lengthList);

}// namespace utils
