#include "views/home/home.h"

#ifdef _DEBUG
int main(int, char *)
#else
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
#endif
{
    const auto win = std::make_unique<Home>();
    return sw::App::MsgLoop();
}
