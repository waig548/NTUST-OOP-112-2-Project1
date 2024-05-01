
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "audio_sys.h"
#include "gui.h"
#include "GUIBoard.h"

inline void errorPopupHandle(bool *p_open, const char *cause)
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Error", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(cause);
        if (ImGui::Button("OK"))
        {
            if (p_open)
                *p_open = false;
        }
        ImGui::EndPopup();
    }
}

inline bool newGameModalHandle(bool *p_open, bool *p_error, std::string *p_errorMsg, int *width, int *height, bool *randomEndpoints, Plumber::GUI::GUIBoard **board)
{
    bool createNewBoard = false;
    Plumber::GUI::GUIBoard *tmp;
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("New Game", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputInt("Columns", width);
        ImGui::InputInt("Rows", height);
        ImGui::Checkbox("Random start and end", randomEndpoints);
        ImGui::Separator();
        if (ImGui::Button("OK"))
        {
            try
            {
                tmp = new Plumber::GUI::GUIBoard(*width, *height, *randomEndpoints);
                createNewBoard = true;
                *p_open = false;
            }
            catch (const std::exception &e)
            {
                *p_error = true;
                *p_errorMsg = e.what();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            *p_open = false;
        }

        if (*p_error)
        {
            ImGui::OpenPopup("Error");
        }
        errorPopupHandle(p_error, p_errorMsg->c_str());

        ImGui::EndPopup();
    }

    if (createNewBoard)
    {
        delete *board;
        *board = tmp;
    }

    return createNewBoard;
}

inline bool loadGameModalHandle(bool *p_open, bool *p_error, std::string *p_errorMsg, std::string *fileName, Plumber::GUI::GUIBoard **board)
{
    bool loadBoard = false;
    Plumber::GUI::GUIBoard *tmp;
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Load Game", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Path to board file", fileName);
        ImGui::Separator();
        if (ImGui::Button("OK"))
        {
            try
            {
                tmp = Plumber::GUI::GUIBoard::loadFromFile(*fileName);
                loadBoard = true;
                *p_open = false;
            }
            catch (const std::exception &e)
            {
                *p_error = true;
                *p_errorMsg = e.what();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            *p_open = false;
        }

        if (*p_error)
        {
            ImGui::OpenPopup("Error");
        }
        errorPopupHandle(p_error, p_errorMsg->c_str());

        ImGui::EndPopup();
    }

    if (loadBoard)
    {
        delete *board;
        *board = tmp;
    }

    return loadBoard;
}

inline void gameFinishedModalHandle(bool *p_open)
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("You win!", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Congratulations! You won the game!");
        if (ImGui::Button("OK"))
        {
            *p_open = false;
        }
        ImGui::EndPopup();
    }
}

int main()
{
    Plumber::AudioSystem::init();
    const auto window = Plumber::GUI::createWindow("Plumber", 800, 600);

    glClearColor(0.25, 0.25, 0.25, 1);

    bool newGamePopup = false;
    bool loadGamePopup = false;
    bool gameFinishedPopup = false;
    bool gameFinishedPopupShown = false;
    int width = 5;
    int height = 4;
    bool randomEndpoints = false;
    std::string fileName;

    bool error = false;
    std::string errorMsg;

    Plumber::GUI::GUIBoard *board = nullptr;

    Plumber::AudioSystem::startBGM();

    while (!Plumber::GUI::windowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Plumber::GUI::beginUI();

        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("Plumber", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Game"))
            {
                if (ImGui::MenuItem("New.."))
                {
                    newGamePopup = true;
                }
                ImGui::SetItemTooltip("Create a new board");
                if (ImGui::MenuItem("Load..."))
                {
                    loadGamePopup = true;
                }
                ImGui::SetItemTooltip("Load a board from file");
                if (ImGui::MenuItem("Close"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::SetItemTooltip("Close the game");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (board)
        {
            board->renderUI();
            board->update();
            if (!gameFinishedPopupShown && board->stable() &&board->finished())
                gameFinishedPopup = true;
            else if (!(board->finished()))
                gameFinishedPopupShown = false;
        }
        else
        {
            ImGui::Text("Use the Game menu to begin.");
        }

        ImGui::End();

        if (newGamePopup)
        {
            ImGui::OpenPopup("New Game");
        }
        if (newGameModalHandle(&newGamePopup, &error, &errorMsg, &width, &height, &randomEndpoints, &board))
            gameFinishedPopupShown = false;

        if (loadGamePopup)
        {
            ImGui::OpenPopup("Load Game");
        }
        if (loadGameModalHandle(&loadGamePopup, &error, &errorMsg, &fileName, &board))
            gameFinishedPopupShown = false;

        if (gameFinishedPopup)
        {
            ImGui::OpenPopup("You win!");
            gameFinishedPopupShown = true;
        }
        gameFinishedModalHandle(&gameFinishedPopup);

        Plumber::GUI::endUI();
    }

    if (board)
    {
        delete board;
        board = nullptr;
    }
    Plumber::GUI::destroyWindow(window);

    Plumber::AudioSystem::dispose();

    return 0;
}