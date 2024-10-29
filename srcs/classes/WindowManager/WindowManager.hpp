#pragma once

#include <GLFW/glfw3.h>
#include <string>

class WindowManager
{
  private:
    static GLFWwindow *window;

    WindowManager() = delete;
    ~WindowManager() = delete;
  public:
    static void InitWindow(const std::string &name, unsigned int width, unsigned int height);
    static void StartUpdateLoop();
    static void StopUpdateLoop();
    static void DestructWindowManager();

    static bool IsKeyPressed(int key);
    static bool IsMouseButtonPressed(int mouseButton);

    static void SetCursorPosCallback(void (*func)(GLFWwindow *window, double xPos, double yPos));
    static void SetCharCallback(void (*func)(GLFWwindow *window, unsigned int character));
};