#pragma once
#include <Windows.h>
#include <atlcomcli.h>
#include <atltypes.h>
#include <oleacc.h>
#include <psapi.h>
#include <string>
#include <uiautomationclient.h>

namespace utils {

void getTextFromPointByMSAA(POINT pt, std::wstring &outStr, CRect &outRc);


void getTextFromPointByUIA(POINT pt, std::wstring &outStr, CRect &outRc);

}// namespace utils
