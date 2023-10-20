#pragma once
#include <Windows.h>
#include <atltypes.h>
#include <oleacc.h>
#include <string>

namespace utils {

void getTextFromPointByMSAA(POINT pt, std::wstring &outStr, CRect &outRc);


void getTextFromPointByUIA(POINT pt, std::wstring &outStr, CRect &outRc);

}// namespace utils
