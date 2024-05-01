#pragma once

#include "../../../libs/glad/include/glad/glad.h"
#include "../Camera/Camera.hpp"
#include <GLFW/glfw3.h>
#include "../WorldClasses/WorldData/WorldData.hpp"
#include "../../globals.hpp"
#include <map>
typedef struct s_data
{
} t_data;

typedef struct s_ftCharacter {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
} t_ftCharacter;

class WindowManager
{
  private:
    GLFWwindow *window;
    Camera camera;
    std::map<char, t_ftCharacter> characters;
    t_data data;

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