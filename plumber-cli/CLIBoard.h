#ifndef PLUMBER_CLI_CLIBOARD_H
#define PLUMBER_CLI_CLIBOARD_H

#include <array>
#include <string>

#include <color.hpp>

#include "Board.h"

namespace Plumber::CLI
{
    class CLIBoard
    {
    public:
        class CLIPipePiece
        {
        public:
            CLIPipePiece(int pipeState);
            CLIPipePiece(const CLIPipePiece &other);
            ~CLIPipePiece();

            virtual CLIPipePiece &operator=(const CLIPipePiece &other);

            std::array<std::string, 3> getIcon() const;
            std::array<dye::R<std::string>, 3> getColorizedIcon() const;

            int getPipeState() const;
            int getPrevState() const;

            void update(int pipeState);

        protected:
            void updateIcon();

            int pipeState, prevState;
            std::array<std::string, 3> icon;
            std::array<dye::R<std::string>, 3> colorizedIcon;
        };

        CLIBoard(int width, int height, bool randomEndpoints = false);
        CLIBoard(int width, int height, const Vec &start, const Vec &end);
        CLIBoard(const CLIBoard &other);
        ~CLIBoard();

        CLIBoard &operator=(const CLIBoard &other);

        int getWidth() const;
        int getHeight() const;

        void printInitialBoard() const;
        void printBoard() const;
        bool finished() const;

        void moveCursorUp();
        void moveCursorRight();
        void moveCursorDown();
        void moveCursorLeft();
        void rotateSelectedCounterclockwise();
        void rotateSelectedClockwise();
        void update();

        static CLIBoard loadFromFile(const std::string &fileName);

    private:
        CLIBoard(const Board &board);
        CLIBoard(const Board &board, const Vec &cursor);

        int indexOf(const Vec &v) const;
        int indexOf(int x, int y) const;
        CLIPipePiece getPieceAt(const Vec &v) const;
        CLIPipePiece getPieceAt(int x, int y) const;

        CLIPipePiece &getPieceAt(const Vec &v);
        CLIPipePiece &getPieceAt(int x, int y);

        void printWater(const Vec &trigger) const;

        void moveCursor(Board::PipePiece::Side dir);

        Board board;
        std::vector<CLIPipePiece> pipePieces;
        Vec cursor;

        static std::vector<CLIPipePiece> parsePieces(const std::vector<int> &pipeStates);
    };
} // namespace CLI

#endif // PLUMBER_CLI_CLIBOARD_H
