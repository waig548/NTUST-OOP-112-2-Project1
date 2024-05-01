#ifndef PLUMBER_GUI_GUIBOARD_H
#define PLUMBER_GUI_GUIBOARD_H

#include <queue>

#include "Board.h"

namespace Plumber::GUI
{
    class GUIBoard
    {
    public:
        class GUIPipePiece
        {
        public:
            GUIPipePiece(const std::string &id, int pipeState);

            std::string getID() const;
            int getState() const;
            int getOpenState() const;
            int getFlags() const;

            void setState(int pipeState);

        private:
            std::string id;
            int state;
        };

        GUIBoard(int width, int height, bool randomEndpoints = false);
        GUIBoard(int width, int height, const Vec &start, const Vec &end);
        GUIBoard(const GUIBoard &other);
        ~GUIBoard();

        GUIBoard &operator=(const GUIBoard &other);

        bool stable() const;
        bool finished() const;

        void renderUI();
        void update();

        static GUIBoard *loadFromFile(const std::string &fileName);

    private:
        GUIBoard(const Board &board);

        int indexOf(int x, int y) const;
        int indexOf(const Vec& v) const;

        void pushUpdates(const Vec& trigger);

        Board board;
        std::vector<GUIPipePiece> pipes;
        std::queue<std::pair<int, int>> queuedUpdates;
    };
} // namespace Plumber::GUI

#endif // PLUMBER_GUI_GUIBOARD_H
