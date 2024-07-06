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

    WorldData world;
    t_data data;

    void start();
    void updateLoop();

    /* process input*/
    void processInput();
    bool isKeyPressed(int key);
    bool isMouseButtonPressed(int mouseButton);
    void updateWireframeMode();
    void updateInfoMode();
    void updateSpeed();
    void updateChat();
    void updateBlock();

    /* text rendering */
    void renderInformations();
    void renderChat();

    /* commands */
    void parseCommand();
    void getBlockCommand(const std::vector<std::string> &commandSplit);
    void teleportCommand(const std::vector<std::string> &commandSplit);

  public:
    WindowManager();
    ~WindowManager();

    /* callback functions */
    void updateCameraAngle(double xPos, double yPos);
    void updateMessage(unsigned int c);
};