#include "cli_util.h"

#include <string>
#include <Windows.h>

#include <color.hpp>

namespace Plumber::CLI
{
    void clearScreen()
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        COORD topLeft = {0, 0};

        std::cout.flush();
        if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        {
            // TODO handle failure
            throw std::runtime_error("Failed to get console screen buffer info");
        }
        DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

        DWORD written;
        FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

        FillConsoleOutputAttribute(hOut, hue::DEFAULT_COLOR, length, topLeft, &written);

        SetConsoleCursorPosition(hOut, topLeft);
    }

    void setCursorPosition(int x, int y)
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        std::cout.flush();
        SetConsoleCursorPosition(hOut, {(SHORT)x, (SHORT)y});
    }

    bool getIntLargerThan(std::istream &stream, int *out, int minimumValue)
    {
        std::string input, tmp;
        std::getline(stream, tmp);
        input += tmp;
        try
        {
            *out = std::stoi(input);
            if (*out >= minimumValue)
                return true;
        }
        catch (std::exception e)
        {
        }
        return false;
    }
} // namespace Plumber::CLI
