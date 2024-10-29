#pragma once

#include <string>
#include "../Camera/Camera.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"

enum InputMode
{
    GAME,
    CHAT
};

class GameLogic
{
    private:
        static Camera camera;
        static InputMode inputMode;
        static WorldData world;

        static void updateChat();
        static void updateWireframeMode();
        static void updateSpeed();
        static void updateInfoMode();
        static void updateBlock();

    public:

        static void Init();
        static void ProcessInput();
        static void UpdateWorldData();

        static void updateCameraAngle(double xPos, double yPos);
        static void updateMessage(unsigned int key);
        static const Camera &GetCamera();
        static const WorldData &GetWorldData();
        static const InputMode &GetInputMode();

        static void SetCameraPosition(const glm::vec3 &pos);

};