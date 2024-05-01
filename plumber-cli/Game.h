#ifndef CLI_GAME_H
#define CLI_GAME_H

#include "CLIBoard.h"

namespace Plumber::CLI
{
    class Game
    {
    public:
        Game(int width, int height, bool randomEndpoints = false);
        Game(std::string fileName);
        ~Game();

        bool run();

    private:
        void loop();

        CLIBoard board;
    };

    enum ValidKeys
    {
        KEY_N = 'N',
        KEY_L = 'L',
        KEY_W = 'W',
        KEY_A = 'A',
        KEY_S = 'S',
        KEY_D = 'D',
        KEY_Q = 'Q',
        KEY_E = 'E',

        KEY_UP = 72,
        KEY_LEFT = 75,
        KEY_DOWN = 80,
        KEY_RIGHT = 77,
        KEY_ESC = 27
    };
} // namespace Plumber::CLI

#endif // CLI_GAME_H
