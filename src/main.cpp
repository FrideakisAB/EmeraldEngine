#include "Log.h"
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Engine/Engine.h"
#include "Memory/MemoryManager.h"

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void errorCallback(int error, const char *description);

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
    logger = new Log();
    logger->Info("Initialization started");

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        logger->Error("Error, GLFW initialization failed");
        delete logger;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *glfwWindow = glfwCreateWindow(1024, 768, "Emerald Engine", nullptr, nullptr);
    if (glfwWindow == nullptr)
    {
        logger->Error("Error, window creation failed");
        glfwTerminate();
        delete logger;

        return -2;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetWindowSizeLimits(glfwWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    if (GLenum err = glewInit(); err != GLEW_OK)
    {
        logger->Error("Error, GLEW initialization failed. %s", glewGetErrorString(err));
        glfwTerminate();
        delete logger;

        return -3;
    }

    if (!GLEW_VERSION_4_5)
    {
        logger->Error("Error, OpenGL 4.5 not supported");
        glfwTerminate();
        delete logger;

        return -4;
    }

    logger->Info("Initialization finished");

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    glClearColor(0.3125f, 0.78125f, 0.46875f, 1.0f);

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(glfwWindow);
    }

    Engine::Destroy();
    Memory::MemoryManager::Get().CheckMemoryLeaks();
    Memory::MemoryManager::Destroy();
    glfwTerminate();
    delete logger;

    return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void errorCallback(int error, const char *description)
{
    logger->Error("GLFW caused error: %s", description);
}
