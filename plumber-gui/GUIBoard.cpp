#include "GUIBoard.h"

#include <string>
#include <sstream>

#include <imgui.h>

#include "audio_sys.h"
#include "gui.h"

namespace Plumber::GUI
{
    GUIBoard::GUIBoard(int width, int height, bool randomEndpoints) : GUIBoard(Board(width, height, randomEndpoints))
    {
    }

    GUIBoard::GUIBoard(int width, int height, const Vec &start, const Vec &end) : GUIBoard(Board(width, height, start, end))
    {
    }

    GUIBoard::GUIBoard(const GUIBoard &other) : GUIBoard(other.board)
    {
    }

    GUIBoard::~GUIBoard()
    {
    }

    GUIBoard &GUIBoard::operator=(const GUIBoard &other)
    {
        board = other.board;
        update();
        return *this;
    }

    bool GUIBoard::stable() const
    {
        return queuedUpdates.empty();
    }

    bool GUIBoard::finished() const
    {
        return board.finished();
    }

    void GUIBoard::renderUI()
    {
        for (int y = 0; y < board.getHeight(); y++)
        {
            for (int x = 0; x < board.getWidth(); x++)
            {
                if (x)
                    ImGui::SameLine();
                auto &pipe = pipes[indexOf(x, y)];
                Texture tex = getPipeTextureOf(pipe.getState());

                int pipeFlags = pipe.getFlags();

                ImVec4 bgColor(0, 0, 0, 0);
                if (pipeFlags & Plumber::Board::PipePiece::Flag::Start)
                    bgColor = {0, 1, 0, 1};
                else if (pipeFlags & Plumber::Board::PipePiece::Flag::End)
                    bgColor = {1, 0, 0, 1};
                else if (pipeFlags & Plumber::Board::PipePiece::Flag::OnPath)
                    bgColor = {0.5, 0, 0.5, 1};

                if (ImGui::ImageButton(pipe.getID().c_str(), (void *)(intptr_t)tex.textureID, ImVec2(tex.width, tex.height), ImVec2(0, 0), ImVec2(1, 1), bgColor))
                {
                    board.rotatePipeAt({x, y});
                    AudioSystem::triggerPipeRotateSound();
                    pushUpdates({x, y});
                }
            }
        }
    }

    void GUIBoard::update()
    {
        while (!queuedUpdates.empty())
        {
            auto updateInfo = queuedUpdates.front();
            queuedUpdates.pop();
            if (pipes[updateInfo.first].getState() == updateInfo.second)
                continue;
            pipes[updateInfo.first].setState(updateInfo.second);
            break;
        }
    }

    GUIBoard *GUIBoard::loadFromFile(const std::string &fileName)
    {
        return new GUIBoard(Board::loadFromFile(fileName));
    }

    GUIBoard::GUIBoard(const Board &board) : board(board), queuedUpdates()
    {
        static std::string pipeIDPattern("#pipe");

        for (int y = 0; y < this->board.getHeight(); y++)
            for (int x = 0; x < this->board.getWidth(); x++)
            {
                std::stringstream ss(pipeIDPattern);
                ss << Vec(x, y);
                pipes.push_back(GUIPipePiece(ss.str(), 0));
            }
        auto pipeStates = board.getPipeStates();
        for (int i = 0; i < pipeStates.size(); i++)
            pipes[i].setState(pipeStates[i] & ~(1 << 4));
        pushUpdates(board.getStart());
    }

    int GUIBoard::indexOf(int x, int y) const
    {
        return board.getWidth() * y + x;
    }

    int GUIBoard::indexOf(const Vec &v) const
    {
        return indexOf(v.getX(), v.getY());
    }

    void GUIBoard::pushUpdates(const Vec &trigger)
    {
        bool doPush = false;
        std::vector<bool> walked(board.getWidth() * board.getHeight(), false);

        board.update();
        auto newStates = board.getPipeStates();

        std::queue<Vec> bfs;
        bfs.push(board.getStart());
        walked[indexOf(board.getStart())] = true;

        while (!bfs.empty())
        {
            Vec cur = bfs.front();
            bfs.pop();
            auto &curPipe = pipes[indexOf(cur)];

            if (cur == trigger)
                doPush = true;
            if (doPush)
                queuedUpdates.push(std::make_pair(indexOf(cur), newStates[indexOf(cur)]));

            for (auto dir : Board::directionOffsets)
            {
                auto next = cur + dir.second;
                if (board.inBounds(next))
                {
                    auto &nextPipe = pipes[indexOf(next)];
                    if (!walked[indexOf(next)] && curPipe.getState() & (1 << dir.first) && nextPipe.getState() & (1 << (dir.first ^ 2)))
                    {
                        walked[indexOf(next)] = true;
                        bfs.push(next);
                    }
                    if (!walked[indexOf(next)] && newStates[indexOf(cur)] & (1 << dir.first) && newStates[indexOf(next)] & (1 << (dir.first ^ 2)))
                    {
                        walked[indexOf(next)] = true;
                        bfs.push(next);
                    }
                }
            }
        }

        for (int i = 0; i < walked.size(); i++)
            if (!walked[i])
                pipes[i].setState(newStates[i]);
    }

    GUIBoard::GUIPipePiece::GUIPipePiece(const std::string &id, int pipeState) : id(id), state(pipeState)
    {
    }

    std::string GUIBoard::GUIPipePiece::getID() const
    {
        return id;
    }

    int GUIBoard::GUIPipePiece::getState() const
    {
        return state;
    }

    int GUIBoard::GUIPipePiece::getOpenState() const
    {
        return state & 0b1111;
    }

    int GUIBoard::GUIPipePiece::getFlags() const
    {
        return state >> 4;
    }

    void GUIBoard::GUIPipePiece::setState(int pipeState)
    {
        state = pipeState;
    }

} // namespace Plumber::GUI
