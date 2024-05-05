#pragma once

#include "../../globals.hpp"
#include "../Camera/Camera.hpp"
#include "../Shader/Shader.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"
#include <GLFW/glfw3.h>
#include <map>
typedef struct s_data
{
    bool infoMode;
    bool wireframeMode;
} t_data;

typedef struct s_ftCharacter
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
} t_ftCharacter;

typedef struct s_textRenderer
{
    std::map<char, t_ftCharacter> characters;
    int pixelSize;
    unsigned int VAO;
    unsigned int VBO;
} t_textRenderer;

class WindowManager
{
  private:
    GLFWwindow *window;
    Camera camera;
    t_textRenderer textRenderer;

    t_data data;

    void start();
    void setupTextRenderer();
    void updateLoop();
    void renderText(Shader &textShader, const std::string &text, float x, float y, float scale, const glm::vec3 &color);
    bool isKeyPressed(int key);
    void updateWireframeMode();
    void updateSpeed();
    void updateInfoMode();

#ifdef DEBUG_MODE
    void DebugWriteMap(const WorldData &world);
    void DebugWritePlayerCoord();
#endif
  public:
    WindowManager();
    ~WindowManager();
};