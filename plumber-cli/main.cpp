#include <iostream>
#include <conio.h>

#include "audio_sys.h"
#include "cli_util.h"
#include "Game.h"

bool newGame(Plumber::CLI::Game **game)
{
    int r, c, random;
    std::string prompt;
    while (true)
    {
        do
            std::cout << "Number of columns (>=3): ";
        while (!Plumber::CLI::getIntLargerThan(std::cin, &c, 3));
        do
            std::cout << "Number of rows (>=3): ";
        while (!Plumber::CLI::getIntLargerThan(std::cin, &r, 3));
        do
            std::cout << "Random start and end? (y/N): ";
        while (!(std::getline(std::cin, prompt)) || (!prompt.empty() && prompt != "y" && prompt != "Y" && prompt != "n" && prompt != "N" && prompt != "exit"));
        random = prompt == "y" || prompt == "Y";
        std::cout << "Board size: " << c << " x " << r << "\nRandom endpoints: " << (random ? "true" : "false") << std::endl;
        std::cout << "Confirm? (Y/n/exit): ";

        std::getline(std::cin, prompt);
        if (prompt == "exit")
            return false;
        if (prompt.empty() || prompt == "y" || prompt == "Y")
            break;
    }
    *game = new Plumber::CLI::Game(c, r, random);
    return true;
}

bool loadGame(Plumber::CLI::Game **game)
{
    std::string fileName, tmp;
    while (!(*game))
    {
        std::cout << "File to load: ";
        std::cin >> fileName;
        std::getline(std::cin, tmp);
        fileName += tmp;
        try
        {
            *game = new Plumber::CLI::Game(fileName);
        }
        catch (std::exception e)
        {
            std::cout << "Failed to load board from file: " << fileName << std::endl;
            std::string prompt;
            do
                std::cout << "Try again? (Y/n): ";
            while (!(std::getline(std::cin, prompt)) || (!prompt.empty() && prompt != "y" && prompt != "Y" && prompt != "n" && prompt != "N"));
            if (prompt == "n" || prompt == "N")
                return false;
        }
    }
    return true;
}

bool mainScreen(Plumber::CLI::Game **game)
{
    int ch;
    while (true)
    {
        Plumber::CLI::clearScreen();
        std::cout << "Welcom to the Plumber game!" << std::endl;
        std::cout << "[N] New game\t[L] Load game from file\t [Esc] Quit" << std::endl;
        ch = _getch();
        if (ch == Plumber::CLI::KEY_ESC)
            return false;
        ch = toupper(ch);

        switch (ch)
        {
        case Plumber::CLI::KEY_N:
            if (newGame(game))
                return true;
            break;
        case Plumber::CLI::KEY_L:
            if (loadGame(game))
                return true;
            break;
        default:
            break;
        }
    }
}

int main()
{
    Plumber::AudioSystem::init();
    Plumber::AudioSystem::startBGM();
    Plumber::CLI::Game *game = nullptr;
    while (mainScreen(&game))
    {
        Plumber::CLI::clearScreen();
        game->run();
        delete game;
        game = nullptr;
    }
    Plumber::AudioSystem::dispose();
    std::cout << "Goodbye!" << std::endl;
    return 0;
}