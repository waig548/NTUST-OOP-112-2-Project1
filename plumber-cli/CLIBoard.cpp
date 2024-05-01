#include "CLIBoard.h"

#include <fstream>
#include <queue>

#include "cli_util.h"

namespace Plumber::CLI
{

    CLIBoard::CLIPipePiece::CLIPipePiece(int pipeState) : prevState(0)
    {
        icon[0] = "###";
        icon[1] = "#P#";
        icon[2] = "###";
        update(pipeState);
    }

    CLIBoard::CLIPipePiece::CLIPipePiece(const CLIPipePiece &other) : CLIPipePiece(other.pipeState)
    {
    }

    CLIBoard::CLIPipePiece::~CLIPipePiece()
    {
    }

    CLIBoard::CLIPipePiece &CLIBoard::CLIPipePiece::operator=(const CLIPipePiece &other)
    {
        update(other.pipeState);
        return *this;
    }

    std::array<std::string, 3> CLIBoard::CLIPipePiece::getIcon() const
    {
        return icon;
    }

    std::array<dye::R<std::string>, 3> CLIBoard::CLIPipePiece::getColorizedIcon() const
    {
        return colorizedIcon;
    }

    int CLIBoard::CLIPipePiece::getPipeState() const
    {
        return pipeState;
    }
    int CLIBoard::CLIPipePiece::getPrevState() const
    {
        return prevState;
    }

    void CLIBoard::CLIPipePiece::update(int pipeState)
    {
        prevState = this->pipeState;
        this->pipeState = pipeState;
        updateIcon();
    }

    void CLIBoard::CLIPipePiece::updateIcon()
    {
        icon[0][1] = (pipeState & (1 << Board::PipePiece::North)) ? 'P' : '#';
        icon[1][2] = (pipeState & (1 << Board::PipePiece::East)) ? 'P' : '#';
        icon[2][1] = (pipeState & (1 << Board::PipePiece::South)) ? 'P' : '#';
        icon[1][0] = (pipeState & (1 << Board::PipePiece::West)) ? 'P' : '#';
        int pipeFlags = pipeState >> 4;
        for (int i = 0; i < 3; i++)
        {
            colorizedIcon[i] = dye::R<std::string>();
            for (int j = 0; j < 3; j++)
            {
                std::string ch(1, icon[i][j]);
                if (pipeFlags & Board::PipePiece::HasWater && ch == "P")
                    colorizedIcon[i] += dye::on_aqua(ch);
                else if (pipeFlags & Board::PipePiece::Selected)
                    colorizedIcon[i] += dye::on_yellow(ch);
                else if (pipeFlags & Board::PipePiece::Start)
                    colorizedIcon[i] += dye::on_green(ch);
                else if (pipeFlags & Board::PipePiece::End)
                    colorizedIcon[i] += dye::on_red(ch);
                else if (pipeFlags & Board::PipePiece::OnPath)
                    colorizedIcon[i] += dye::purple(ch);
                else
                    colorizedIcon[i] += dye::vanilla(ch);
            }
        }
    }

    CLIBoard::CLIBoard(int width, int height, bool randomEndpoints) : CLIBoard(Board(width, height, randomEndpoints))
    {
    }

    CLIBoard::CLIBoard(int width, int height, const Vec &start, const Vec &end) : CLIBoard(Board(width, height, start, end))
    {
    }

    CLIBoard::CLIBoard(const CLIBoard &other) : CLIBoard(other.board, other.cursor)
    {
    }

    CLIBoard::~CLIBoard()
    {
    }

    CLIBoard &CLIBoard::operator=(const CLIBoard &other)
    {
        board = Board(other.board);
        pipePieces = parsePieces(board.getPipeStates());
        return *this;
    }

    int CLIBoard::getWidth() const
    {
        return board.getWidth();
    }

    int CLIBoard::getHeight() const
    {
        return board.getHeight();
    }

    void CLIBoard::printInitialBoard() const
    {
        printWater(board.getStart());
    }

    void CLIBoard::printBoard() const
    {
        for (int y = 0; y < board.getHeight(); y++)
            for (int x = 0; x < board.getWidth(); x++)
            {
                auto &p = getPieceAt(x, y);
                for (int iy = 0; iy < 3; iy++)
                {
                    CLI::setCursorPosition(x * 4, y * 4 + iy);
                    std::cout << p.getColorizedIcon()[iy];
                }
            }
        std::cout.flush();
    }

    bool CLIBoard::finished() const
    {
        return board.finished();
    }

    void CLIBoard::moveCursorUp()
    {
        moveCursor(Board::PipePiece::North);
    }

    void CLIBoard::moveCursorRight()
    {
        moveCursor(Board::PipePiece::East);
    }

    void CLIBoard::moveCursorDown()
    {
        moveCursor(Board::PipePiece::South);
    }

    void CLIBoard::moveCursorLeft()
    {
        moveCursor(Board::PipePiece::West);
    }

    void CLIBoard::rotateSelectedCounterclockwise()
    {
        board.rotatePipeAt(cursor, true);
        update();
        printWater(cursor);
    }

    void CLIBoard::rotateSelectedClockwise()
    {
        board.rotatePipeAt(cursor);
        update();
        printWater(cursor);
    }

    void CLIBoard::update()
    {
        board.select(cursor);
        board.update();
        auto pipeStates = board.getPipeStates();
        for (int i = 0; i < pipeStates.size(); i++)
        {
            pipePieces[i].update(pipeStates[i]);
        }
    }

    CLIBoard::CLIBoard(const Board &board) : CLIBoard(board, Vec())
    {
    }
    CLIBoard::CLIBoard(const Board &board, const Vec &cursor) : board(board), cursor(cursor)
    {
        pipePieces = parsePieces(this->board.getPipeStates());
        update();
    }

    int CLIBoard::indexOf(const Vec &v) const
    {
        return indexOf(v.getX(), v.getY());
    }

    int CLIBoard::indexOf(int x, int y) const
    {
        return y * board.getWidth() + x;
    }

    CLIBoard::CLIPipePiece CLIBoard::getPieceAt(const Vec &v) const
    {
        return pipePieces[indexOf(v)];
    }

    CLIBoard::CLIPipePiece &CLIBoard::getPieceAt(const Vec &v)
    {
        return pipePieces[indexOf(v)];
    }

    CLIBoard::CLIPipePiece CLIBoard::getPieceAt(int x, int y) const
    {
        return pipePieces[indexOf(x, y)];
    }

    CLIBoard::CLIPipePiece &CLIBoard::getPieceAt(int x, int y)
    {
        return pipePieces[indexOf(x, y)];
    }

    void CLIBoard::printWater(const Vec &trigger) const
    {
        static auto printPipe = [](const CLIPipePiece &pipe, const Vec &pos) -> void
        {
            auto icon = pipe.getIcon();

            for (int iy = 0; iy < 3; iy++)
            {
                CLI::setCursorPosition(pos.getX() * 4, pos.getY() * 4 + iy);
                std::cout << pipe.getColorizedIcon()[iy];
                if (pipe.getPipeState() & (Board::PipePiece::Flag::HasWater << 4))
                {
                    for (int ix = 0; ix < 3; ix++)
                    {
                        CLI::setCursorPosition(pos.getX() * 4 + ix, pos.getY() * 4 + iy);
                        if (icon[iy][ix] == 'P')
                            std::cout << dye::on_aqua(icon[iy][ix]);
                    }
                }
            }
        };

        bool doDelay = false;
        std::vector<bool> walked(board.getWidth() * board.getHeight(), false);

        std::queue<Vec> bfs;
        bfs.push(board.getStart());
        walked[indexOf(board.getStart())] = true;

        while (!bfs.empty())
        {
            Vec cur = bfs.front();
            bfs.pop();
            auto &curPipe = pipePieces[indexOf(cur)];
            

            if (cur == trigger)
                doDelay = true;
            if (doDelay)
                Sleep(5);

            printPipe(curPipe, cur);

            for (auto dir : Board::directionOffsets)
            {
                auto next = cur + dir.second;
                if (board.inBounds(next))
                {
                    auto &nextPipe = pipePieces[indexOf(next)];
                    if (!walked[indexOf(next)] && curPipe.getPrevState() & (1 << dir.first) && nextPipe.getPrevState() & (1 << (dir.first ^ 2)))
                    {
                        walked[indexOf(next)] = true;
                        bfs.push(next);
                    }
                    if (!walked[indexOf(next)] && curPipe.getPipeState() & (1 << dir.first) && nextPipe.getPipeState() & (1 << (dir.first ^ 2)))
                    {
                        walked[indexOf(next)] = true;
                        bfs.push(next);
                    }
                }
            }
        }

        for (int i = 0; i < walked.size(); i++)
            if (!walked[i])
                printPipe(pipePieces[i], Vec(i % board.getWidth(), i / board.getWidth()));
    }

    void CLIBoard::moveCursor(Board::PipePiece::Side dir)
    {
        auto target = cursor + Board::directionOffsets.at(dir);
        if (board.inBounds(target))
        {
            cursor = target;
            board.select(cursor);
        }
    }

    CLIBoard CLIBoard::loadFromFile(const std::string &fileName)
    {
        return CLIBoard(Board::loadFromFile(fileName));
    }

    std::vector<CLIBoard::CLIPipePiece> CLIBoard::parsePieces(const std::vector<int> &pipeStates)
    {
        std::vector<CLIPipePiece> res;
        for (auto e : pipeStates)
            res.push_back(CLIPipePiece(e));
        return res;
    }

} // namespace Plumber::CLI
