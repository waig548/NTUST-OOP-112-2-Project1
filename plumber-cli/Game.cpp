#include "Game.h"

#include <conio.h>

#include "audio_sys.h"
#include "cli_util.h"

namespace Plumber::CLI
{

    Game::Game(int width, int height, bool randomEndpoints) : board(std::move(CLIBoard(width, height, randomEndpoints)))
    {
    }

    Game::Game(std::string fileName) : board(CLIBoard::loadFromFile(fileName))
    {
    }

    Game::~Game()
    {
    }

    bool Game::run()
    {
        CLI::clearScreen();
        loop();
        return true;
    }

    void Game::loop()
    {
        board.printInitialBoard();
        while (true)
        {
            board.update();
            board.printBoard();

            CLI::setCursorPosition(0, 4 * board.getHeight());
            std::cout << "Legends:" << std::endl;
            std::cout << dye::on_green("Start") << "\t" << dye::on_red("End") << "\t" << dye::on_aqua("Water") << "\t" << dye::on_yellow("Cursor") << "\t" << dye::purple("Solution") << std::endl;
            std::cout << "Controls:" << std::endl;
            std::cout << "W/\u2191: Move cursor up\tA/\u2190: Move cursor left\tS/\u2193: Move cursor down\tD/\u2192: Move cursor right" << std::endl;
            std::cout << "Q: Rotate pipe counterclockwise\t\tE: Rotate pipe clockwise\tEsc: Quit" << std::endl;

            if (board.finished())
            {
                CLI::setCursorPosition(0, 4 * board.getHeight() - 1);
                std::cout << dye::on_yellow("Congratulations! You won the game!") << std::endl;
            }

            int ch = _getch();
            if (ch == KEY_ESC)
            {
                if (!board.finished())
                {
                    CLI::setCursorPosition(0, 4 * board.getHeight() - 1);
                    std::cout << dye::on_grey("Game quitted.") << std::endl;
                }
                break;
            }
            if (ch == 0 || ch == 224)
                ch = _getch();
            ch = toupper(ch);
            switch (ch)
            {
            case KEY_W:
            case KEY_UP:
                board.moveCursorUp();
                break;
            case KEY_A:
            case KEY_LEFT:
                board.moveCursorLeft();
                break;
            case KEY_S:
            case KEY_DOWN:
                board.moveCursorDown();
                break;
            case KEY_D:
            case KEY_RIGHT:
                board.moveCursorRight();
                break;
            case KEY_Q:
                Plumber::AudioSystem::triggerPipeRotateSound();
                board.rotateSelectedCounterclockwise();
                break;
            case KEY_E:
                Plumber::AudioSystem::triggerPipeRotateSound();
                board.rotateSelectedClockwise();
                break;
            default:
                break;
            }
        }
        CLI::setCursorPosition(0, 4 * board.getHeight() + 5);
        std::cout << "Press any key to return to main screen...";
        _getch();
    }

} // namespace Plumber::CLI