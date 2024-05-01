#include "gui.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static std::map<int, Plumber::GUI::Texture> pipeTextures;

static bool loadTextureFromFile(const char *filename, GLuint *outTexture, int *outWidth, int *outHeight)
{
    int imageWidth = 0;
    int imageHeight = 0;
    unsigned char *image_data = stbi_load(filename, &imageWidth, &imageHeight, NULL, 4);
    if (!image_data)
        return false;

    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *outTexture = imageTexture;
    *outWidth = imageWidth;
    *outHeight = imageHeight;

    return true;
}

namespace Plumber::GUI
{
    GLFWwindow *createWindow(const char *title, int width, int height)
    {
        GLFWwindow *window;

        glfwSetErrorCallback([](int error, const char *description)
                             { std::fprintf(stderr, "GLFW Error %d: %s\n", error, description); });

        assert(glfwInit() && "Failed to initialize GLFW ._.");

        // GL 4.6 + GLSL 460
        const char *glsl_version = "#version 460";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        // setup window
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        assert(window && "Failed to create window ._.");
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        // setup resize callback
        glfwSetWindowSizeCallback(window, [](GLFWwindow *, int width, int height)
                                  { glViewport(0, 0, width, std::max<int>(height, 1)); });

        // setup GLAD
        assert(gladLoadGL(glfwGetProcAddress) && "Something went wrong with glad ._.");

        // setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsClassic();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.FontDefault = io.Fonts->AddFontDefault();
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSansTC-Black.ttf", 20, nullptr, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->Build();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        loadTextures();

        return window;
    }

    bool windowShouldClose(GLFWwindow *window)
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
        return glfwWindowShouldClose(window);
    }

    void destroyWindow(GLFWwindow *window)
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void beginUI()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void endUI()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void loadTextures()
    {
        static std::string filenamePattern("pipe_");
        std::filesystem::path texturesPath("assets/textures");

        for (auto const &file : std::filesystem::directory_iterator(texturesPath))
        {
            GLuint textureID;
            int width, height;
            try
            {
                int id = std::stoi(file.path().stem().string().substr(filenamePattern.size()));
                if (loadTextureFromFile(file.path().string().c_str(), &textureID, &width, &height))
                    pipeTextures.try_emplace(id, Texture{textureID, width, height});
            }
            catch (const std::exception &e)
            {
                std::cerr << "Something wend wrong when loading texture file: " << file << std::endl;
                std::cerr << e.what() << std::endl;
            }
        }
    }

    Texture getPipeTextureOf(int pipeState)
    {
        return pipeTextures.at(pipeState & 0b11111);
    }

} // namespace Plumber::GUI
