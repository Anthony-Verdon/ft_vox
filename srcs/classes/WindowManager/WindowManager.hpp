#pragma once

#include "../../../libs/glad/include/glad/glad.h"
#include "../Camera/Camera.hpp"
#include <GLFW/glfw3.h>
#include "../WorldClasses/WorldData/WorldData.hpp"
#include "../../globals.hpp"
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
    
#ifdef DEBUG_MODE
    void DebugWriteMap(const WorldData &world);
    void DebugWritePlayerCoord();
#endif
  public:
    WindowManager();
    ~WindowManager();
};