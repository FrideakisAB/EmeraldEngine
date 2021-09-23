#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *glfwWindow = glfwCreateWindow(1024, 768, "Emerald Engine", nullptr, nullptr);
    if (glfwWindow == nullptr)
    {
        glfwTerminate();

        return -2;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetWindowSizeLimits(glfwWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSwapInterval(0);

    glewExperimental = GL_TRUE;
    if (GLenum err = glewInit(); err != GLEW_OK)
    {
        glfwTerminate();

        return -3;
    }

    if (!GLEW_VERSION_4_4)
    {
        glfwTerminate();

        return -4;
    }

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

    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
