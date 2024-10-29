#pragma once

#include <string>
#include "../Camera/Camera.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"

enum InputMode
{
    GAME,
    CHAT
};

typedef struct s_ChatData
{
    std::string message;
    float lastMessageTimeStamp;
    std::string lastMessage;
} t_ChatData;

class GameLogic
{
    private:
        static Camera camera;
        static InputMode inputMode;
        static WorldData world;
        static t_ChatData chat;
        

        static void updateChat();
        static void updateWireframeMode();
        static void updateSpeed();
        static void updateInfoMode();
        static void updateBlock();
        static void parseCommand();
        static void getBlockCommand(const std::vector<std::string> &commandSplit);
        static void teleportCommand(const std::vector<std::string> &commandSplit);

    public:

        static void Init();
        static void ProcessInput();
        static void UpdateWorldData();

        static void updateCameraAngle(double xPos, double yPos);
        static void updateMessage(unsigned int key);
        static const Camera &GetCamera();
        static const WorldData &GetWorldData();
        static const InputMode &GetInputMode();
        static const t_ChatData &GetChatData();

};