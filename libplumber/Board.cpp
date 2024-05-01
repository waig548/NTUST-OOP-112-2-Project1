#include "Board.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <random>
#include <queue>
#include <stack>

namespace Plumber
{
    Board::Board(int width, int height, bool randomEndpoints) : width(width), height(height), start(), end(width - 1, height - 1)
    {
        if (width < 3 || height < 3)
            throw std::invalid_argument("Board size must be at least 3x3.");
        if (randomEndpoints)
        {
            auto res = generateEndpoints(width, height);
            start = res.first;
            end = res.second;
        }
        auto res = generateBoard(width, height, start, end);
        content = res.first;
        solution = res.second;
        postGenerationActions();
    }

    Board::Board(int width, int height, const Vec &start, const Vec &end) : width(width), height(height), start(0, 0), end(width - 1, height - 1)
    {
        if (width < 3 || height < 3)
            throw std::invalid_argument("Board size must be at least 3x3.");
        if (start != end)
        {
            if (onEdges(start, width, height))
                this->start = start;
            if (onEdges(end, width, height))
                this->end = end;
        }
        auto res = generateBoard(this->width, this->height, this->start, this->end);
        content = res.first;
        solution = res.second;
        postGenerationActions();
    }

    Board::Board(int width, int height, const std::vector<int> &pipeStates) : Board(width, height, Vec(), Vec(), pipeStates)
    {
    }

    Board::Board(int width, int height, const Vec &start, const Vec &end, const std::vector<int> &pipeStates) : width(width), height(height), start(0, 0), end(width - 1, height - 1)
    {
        if (start != end)
        {
            if (onEdges(start, width, height))
                this->start = start;
            if (onEdges(end, width, height))
                this->end = end;
        }
        for (auto state : pipeStates)
        {
            content.push_back(PipePiece(state));
        }
        markEndpoints();
        update();
    }

    Board::Board(const Board &other) : width(other.width), height(other.height), start(other.start), end(other.end)
    {
        for (auto e : other.content)
        {
            content.push_back(PipePiece(e));
        }
        for (auto e : other.solution)
        {
            solution.push_back(e);
        }
    }

    Board::~Board()
    {
    }

    Board &Board::operator=(const Board &other)
    {
        width = other.width;
        height = other.height;
        start = other.start;
        end = other.end;
        content.clear();
        for (auto e : other.content)
            content.push_back(PipePiece(e));
        solution.clear();
        for (auto e : other.solution)
            solution.push_back(e);
        return *this;
    }

    int Board::getWidth() const
    {
        return width;
    }

    int Board::getHeight() const
    {
        return height;
    }

    Vec Board::getStart() const
    {
        return start;
    }

    std::vector<int> Board::getPipeStates() const
    {
        std::vector<int> res;
        for (auto p : content)
        {
            res.push_back(p.getOpenState() | (p.getFlags() << 4));
        }
        return res;
    }

    bool Board::finished() const
    {
        return content[indexOf(end)].hasFlag(PipePiece::Flag::HasWater);
    }

    void Board::rotatePipeAt(const Vec &v, bool counterClockwise)
    {
        if (counterClockwise)
            content[indexOf(v)].rotateCounterClockwise();
        else
            content[indexOf(v)].rotateClockwise();
    }

    void Board::select(const Vec &v)
    {
        if (!inBounds(v))
            return;
        for (auto &e : content)
            e.clearFlags(PipePiece::Flag::Selected);
        content[indexOf(v)].setFlags(PipePiece::Flag::Selected);
        content[indexOf(v)].update();
    }

    void Board::update()
    {
        for (auto e : content)
            e.update();
        populateWater();
    }

    Board Board::loadFromFile(const std::string &fileName)
    {
        std::ifstream f(fileName);
        if (!f.good())
            throw std::runtime_error("Failed to open file.");
        int r, c;
        f >> r >> c;
        if (r < 0 || c < 0)
            throw std::invalid_argument("Invalid board size.");
        f.ignore(1, '\n');
        // TODO validate board content
        std::vector<int> pipeStates;
        for (int y = 0; y < r; y++)
        {
            std::vector<std::string> row(c, "");
            for (int l = 0; l < 3; l++)
            {
                for (int x = 0; x < c; x++)
                {
                    std::string tmp;
                    f >> tmp;
                    row[x] += tmp;
                }
            }
            for (auto s : row)
            {
                int raw = 0;
                for (int i = 0; i < 4; i++)
                    raw |= (s[2 * i + 1] == 'P') << i;
                int state = raw & 0b0001;
                state |= (bool)(raw & 0b0100) << 1;
                state |= (bool)(raw & 0b1000) << 2;
                state |= (bool)(raw & 0b0010) << 3;
                pipeStates.push_back(state);
            }
        }
        return Board(c, r, pipeStates);
    }

    bool Board::inBounds(const Vec &v) const
    {
        return inBounds(v, width, height);
    }

    int Board::indexOf(const Vec &v) const
    {
        return indexOf(v, width, height);
    }

    void Board::shuffle()
    {
        srand((unsigned int)time(nullptr));
        for (auto &pipe : content)
            pipe.setRotation(rand() % 4);
    }

    void Board::postGenerationActions()
    {
        if (!solution.empty())
            for (int i = 0; i < solution.size(); i++)
                content[indexOf(solution[i])].setFlags(PipePiece::Flag::OnPath);
        markEndpoints();
        do
        {
            shuffle();
            update();
        } while (finished());
    }

    void Board::markEndpoints()
    {
        content[indexOf(start)].setFlags(PipePiece::Flag::Start);
        content[indexOf(end)].setFlags(PipePiece::Flag::End);
    }

    void Board::populateWater()
    {
        for (auto &p : content)
            p.clearFlags(PipePiece::Flag::HasWater);
        std::queue<Vec> bfs;
        bfs.push(start);
        while (!bfs.empty())
        {
            Vec cur = bfs.front();
            bfs.pop();
            auto &curPipe = content[indexOf(cur)];
            curPipe.setFlags(PipePiece::Flag::HasWater);
            for (auto dir : directionOffsets)
            {
                auto next = cur + dir.second;
                if (inBounds(next))
                {
                    auto &nextPipe = content[indexOf(next)];
                    if (curPipe.isOpenOn(dir.first) && nextPipe.isOpenOn((PipePiece::Side)(dir.first ^ 2)) && !(nextPipe.hasFlag(PipePiece::Flag::HasWater)))
                        bfs.push(next);
                }
            }
        }
    }

    std::pair<std::vector<Board::PipePiece>, std::vector<Vec>> Board::generateBoard(int width, int height, const Vec &start, const Vec &end)
    {
        static auto countOpenings = [](int n) -> int
        {
            int c = 0;
            for (int i = 0; i < 4; i++)
                if (n & (1 << i))
                    c++;
            return c;
        };

        auto random = std::default_random_engine((unsigned int)time(nullptr));

        std::vector<int> maze(width * height, 0);

        static auto getCellIndexAt = [&width](const Vec &v) -> int
        {
            return v.getY() * width + v.getX();
        };

        static auto getCellAt = [&maze, &width](const Vec &v) -> auto &
        {
            return maze[v.getY() * width + v.getX()];
        };

        std::vector<Vec> parents(width * height);
        static auto traceSolution = [&parents](const Vec &start, const Vec &end) -> std::vector<Vec>
        {
            std::vector<Vec> path;
            Vec cur = end;
            while (cur != start)
            {
                path.push_back(cur);
                cur = parents[getCellIndexAt(cur)];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        };

        std::stack<Vec> st;
        Vec cur = start;
        st.push(cur);
        getCellAt(cur) |= 128;

        while (!st.empty())
        {
            cur = st.top();
            st.pop();
            auto randomDirs = directions;
            std::shuffle(randomDirs.begin(), randomDirs.end(), random);
            for (auto dir : randomDirs)
            {
                auto next = cur + directionOffsets.at(dir);
                if (inBounds(next, width, height) && !(getCellAt(next) & 128))
                {
                    getCellAt(cur) |= 1 << dir;
                    getCellAt(next) |= 128 | 1 << (dir ^ 2);
                    parents[getCellIndexAt(next)] = cur;
                    st.push(next);
                }
            }
        }

        for (auto &e : maze)
        {
            int r = random() % 100;
            if (r < 5)
                r = 4;
            else if (r < 30)
                r = 3;
            else
                r = 2;
            while (countOpenings(e) < r)
                e |= 1 << (rand() % 4);
        }

        std::vector<Board::PipePiece> res;
        for (auto e : maze)
            res.push_back(PipePiece(e & 0b1111));

        auto solution = traceSolution(start, end);

        return std::make_pair(res, solution);
    }

    std::pair<Vec, Vec> Board::generateEndpoints(int width, int height)
    {
        srand((unsigned int)time(nullptr));
        std::array<Vec, 2> points;
        do
        {
            for (int i = 0; i < 2; i++)
            {
                int alignment = rand() % 4;
                if (alignment >> 1)
                    points[i] = Vec(rand() % width, (height - 1) * (alignment & 1));
                else
                    points[i] = Vec((width - 1) * (alignment & 1), rand() % height);
            }
        } while (points[0] == points[1]);

        return std::make_pair(points[0], points[1]);
    }

    bool Board::inBounds(const Vec &v, int width, int height)
    {
        return v.getX() >= 0 && v.getX() < width && v.getY() >= 0 && v.getY() < height;
    }

    bool Board::onEdges(const Vec &v, int width, int height)
    {
        return v.getX() == 0 || v.getX() == width - 1 || v.getY() == 0 || v.getY() == height - 1;
    }

    int Board::indexOf(const Vec &v, int width, int height)
    {
        if (!inBounds(v, width, height))
            return -1;
        return v.getY() * width + v.getX();
    }

    Board::PipePiece::PipePiece(const PipePiece &other) : PipePiece(other.initialOpenState, other.rotation, other.flags)
    {
    }

    Board::PipePiece::~PipePiece()
    {
    }

    Board::PipePiece &Board::PipePiece::operator=(const PipePiece &other)
    {
        initialOpenState = other.initialOpenState;
        setRotation(other.rotation);
        flags = other.flags;
        return *this;
    }

    void Board::PipePiece::rotateClockwise()
    {
        rotation = (++rotation) & 0b11;
        update();
    }

    void Board::PipePiece::rotateCounterClockwise()
    {
        rotation = (--rotation) & 0b11;
        update();
    }

    int Board::PipePiece::getFlags() const
    {
        return flags;
    }

    int Board::PipePiece::getOpenState() const
    {
        return openState;
    }

    bool Board::PipePiece::hasFlag(Flag flag) const
    {
        return flags & flag;
    }

    void Board::PipePiece::setRotation(int rotation)
    {
        this->rotation = rotation & 0b11;
        update();
    }

    void Board::PipePiece::setFlags(int flags)
    {
        this->flags |= flags;
    }

    void Board::PipePiece::clearFlags(int flags)
    {
        this->flags &= ~flags;
    }

    bool Board::PipePiece::isOpenOn(Side side) const
    {
        return openState & (1 << side);
    }

    Board::PipePiece::PipePiece(int initialOpenState) : PipePiece(initialOpenState, 0)
    {
    }

    Board::PipePiece::PipePiece(int initialOpenState, int rotation) : PipePiece(initialOpenState, rotation, 0)
    {
    }

    void Board::PipePiece::update()
    {
        openState = calcOpenState(initialOpenState, rotation);
    }

    Board::PipePiece::PipePiece(int initialOpenState, int rotation, int flags) : initialOpenState(initialOpenState), rotation(rotation & 0b11), flags(flags)
    {
        update();
    }

    int Board::PipePiece::calcOpenState(int initialOpenState, int rotation)
    {
        return ((initialOpenState << rotation) & 0xf) | (initialOpenState << rotation) >> 4;
    }

    const std::vector<Board::PipePiece::Side> Board::directions{
        Board::PipePiece::North,
        Board::PipePiece::East,
        Board::PipePiece::South,
        Board::PipePiece::West};

    const std::map<Board::PipePiece::Side, Vec> Board::directionOffsets{
        {Board::PipePiece::North, Vec(0, -1)},
        {Board::PipePiece::East, Vec(1, 0)},
        {Board::PipePiece::South, Vec(0, 1)},
        {Board::PipePiece::West, Vec(-1, 0)}};

} // namespace Plumber
