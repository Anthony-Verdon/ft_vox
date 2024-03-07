#pragma once

#include "../../../libs/glad/include/glad/glad.h"
#include "../Camera/Camera.hpp"
#include <GLFW/glfw3.h>

class WindowManager
{
  private:
    GLFWwindow *window;
    Camera camera;

    void start();
    void updateLoop();
    bool isKeyPressed(int key);
    void updateWireframeMode();
    void updateSpeed();

  public:
    WindowManager();
    ~WindowManager();
};