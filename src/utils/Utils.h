#pragma once
#include <Point.h>
#include <string>

namespace utils
{

POINT getMousePosition();

std::wstring getTextByUIA(POINT pos);

std::wstring getTextByMSAA(POINT pos);

} // namespace utils