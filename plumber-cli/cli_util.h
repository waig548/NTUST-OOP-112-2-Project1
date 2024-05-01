#ifndef PLUMBER_CLI_CLI_UTIL_H
#define PLUMBER_CLI_CLI_UTIL_H

#include <iostream>

namespace Plumber::CLI
{
    void clearScreen();
    void setCursorPosition(int x, int y);
    bool getIntLargerThan(std::istream &stream, int *out, int minimumValue);
} // namespace Plumber::CLI

#endif // Plumber::CLI_CLI_UTIL_H