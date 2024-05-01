#if !defined(PLUMBER_GUI_GUI_H)
#define PLUMBER_GUI_GUI_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace Plumber::GUI
{
    typedef struct
    {
        GLuint textureID;
        int width;
        int height;
    } Texture;

    GLFWwindow *createWindow(const char *title, int width, int height);
    bool windowShouldClose(GLFWwindow *window);
    void destroyWindow(GLFWwindow *window);
    void beginUI();
    void endUI();
    void loadTextures();
    Texture getPipeTextureOf(int pipeState);
} // namespace Plumber::GUI

#endif // PLUMBER_GUI_GUI_H
