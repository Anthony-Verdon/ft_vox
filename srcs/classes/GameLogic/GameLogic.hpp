#pragma once

#include <string>
#include "../Camera/Camera.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"

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

class GameLogic
{
    private:
        

        static void updateChat();
        static void updateWireframeMode();
        static void updateSpeed();
        static void updateInfoMode();
        static void updateBlock();
        static void parseCommand();
        static void getBlockCommand(const std::vector<std::string> &commandSplit);
        static void teleportCommand(const std::vector<std::string> &commandSplit);

    public:
        static Camera camera;
        static t_data data;
        static WorldData world;

        static void Init();
        static void ProcessInput();
        static void UpdateCameraAngle();
    
        static void updateCameraAngle(double xPos, double yPos);
        static void updateMessage(unsigned int key);

};