#ifndef PLUMBER_BOARD_H
#define PLUMBER_BOARD_H

#include <vector>
#include <map>
#include <memory>
#include "Vec.h"
#include "util.h"

namespace Plumber
{
    class Board
    {
    public:
        class PipePiece
        {
        public:
            enum Side
            {
                North,
                East,
                South,
                West
            };

            enum Flag
            {
                HasWater = 1 << 0,
                OnPath = 1 << 1,
                Start = 1 << 2,
                End = 1 << 3,
                Selected = 1 << 4
            };

            PipePiece(int initialOpenState);
            PipePiece(int initialOpenState, int rotation);
            PipePiece(const PipePiece &other);
            ~PipePiece();

            PipePiece &operator=(const PipePiece &other);

            int getFlags() const;
            int getOpenState() const;
            bool hasFlag(Flag flag) const;
            bool isOpenOn(Side side) const;

            void rotateClockwise();
            void rotateCounterClockwise();
            void setRotation(int rotation);
            void setFlags(int flags);
            void clearFlags(int flags);
            void update();

        protected:
            int rotation, openState, flags;
            int initialOpenState;

        private:
            PipePiece(int initialOpenState, int rotation, int flags);

            static int calcOpenState(int initialOpenState, int rotation);
        };

        Board(int width, int height, bool randomEndpoints = false);
        Board(int width, int height, const Vec &start, const Vec &end);
        Board(int width, int height, const std::vector<int> &pipeStates);
        Board(int width, int height, const Vec &start, const Vec &end, const std::vector<int> &pipeStates);
        Board(const Board &other);
        ~Board();

        Board &operator=(const Board &other);

        int getWidth() const;
        int getHeight() const;
        Vec getStart() const;

        std::vector<int> getPipeStates() const;

        bool inBounds(const Vec &v) const;
        bool finished() const;

        void rotatePipeAt(const Vec &v, bool counterClockwise = false);
        void select(const Vec &v);
        void update();

        static Board loadFromFile(const std::string &fileName);

        static const std::vector<PipePiece::Side> directions;
        static const std::map<PipePiece::Side, Vec> directionOffsets;

    private:
        int indexOf(const Vec &v) const;

        void shuffle();
        void postGenerationActions();
        void markEndpoints();
        void populateWater();

        int width, height;
        Vec start, end;
        std::vector<PipePiece> content;
        std::vector<Vec> solution;

        static std::pair<std::vector<PipePiece>, std::vector<Vec>> generateBoard(int width, int height, const Vec &start, const Vec &end);
        static std::pair<Vec, Vec> generateEndpoints(int width, int height);

        static bool inBounds(const Vec &v, int width, int height);
        static bool onEdges(const Vec &v, int width, int height);
        static int indexOf(const Vec &v, int width, int height);
    };

} // namespace Plumber

#endif // PLUMBER_BOARD_H
