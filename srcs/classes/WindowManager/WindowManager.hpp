#pragma once

#include "../../globals.hpp"
#include "../Camera/Camera.hpp"
#include "../Shader/Shader.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"
#include <GLFW/glfw3.h>
#include <map>

enum InputMode
{
    GAME,
    CHAT
};

typedef struct s_data
{
    InputMode inputMode;
    bool infoMode;
    bool wireframeMode;
    std::string message;
    float lastMessageTimeStamp;
    std::string lastMessage;
} t_data;

class WindowManager
{
  private:
    GLFWwindow *window;
    Camera camera;
    t_data data;

    void start();
    void updateLoop();

    void loadSkybox(unsigned int *VAO, unsigned int *VBO);
    void processInput();
    bool isKeyPressed(int key);
    void updateChat();
    void updateWireframeMode();
    void updateSpeed();
    void updateInfoMode();
    void renderInformations();
    void renderChat();

  public:
    WindowManager();
    ~WindowManager();

    void updateCameraAngle(double xPos, double yPos);
    void updateMessage(unsigned int c);
};