#include "GameLogic.hpp"
#include "../WindowManager/WindowManager.hpp"
#include "../GameRenderer/GameRenderer.hpp"
#include "../Utils/Utils.hpp"
#include "../ChatLogic/ChatLogic.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera GameLogic::camera;
InputMode GameLogic::inputMode = GAME;
WorldData GameLogic::world;

void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void character_callback(GLFWwindow *window, unsigned int character);



void GameLogic::Init()
{
    camera.setPosition({8, 70, 8});

    WindowManager::SetCursorPosCallback(mouse_callback);
    WindowManager::SetCharCallback(character_callback);
}

void GameLogic::ProcessInput()
{
    if (WindowManager::IsKeyPressed(GLFW_KEY_ESCAPE))
        WindowManager::StopUpdateLoop();

    static bool keyEnable = true;
    if (WindowManager::IsKeyPressed(GLFW_KEY_ENTER))
    {
        if (keyEnable == true)
        {
            if (inputMode == CHAT)
                ChatLogic::SendMessage();

            if (inputMode == CHAT)
                inputMode = GAME;
            else if (inputMode == GAME)
                inputMode = CHAT;
        }
        keyEnable = false;
    }
    else
        keyEnable = true;

    switch (inputMode)
    {
    case GAME: {
        updateWireframeMode();
        updateSpeed();
        updateInfoMode();
        updateBlock();
        int frontAxis = WindowManager::IsKeyPressed(GLFW_KEY_W) - WindowManager::IsKeyPressed(GLFW_KEY_S);
        int rightAxis = WindowManager::IsKeyPressed(GLFW_KEY_D) - WindowManager::IsKeyPressed(GLFW_KEY_A);
        int upAxis = WindowManager::IsKeyPressed(GLFW_KEY_SPACE) - WindowManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT);
        camera.updatePosition(frontAxis, rightAxis, upAxis);
        break;
    }
    case CHAT:
        updateChat();
        break;
    default:
        break;
    }
}

void GameLogic::updateChat()
{
    static bool keyEnable = true;

    if (WindowManager::IsKeyPressed(GLFW_KEY_BACKSPACE))
    {
        if (keyEnable == true)
            ChatLogic::EraseLastCharacter();
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void GameLogic::updateWireframeMode()
{
    static bool keyEnable = true;
    if (WindowManager::IsKeyPressed(GLFW_KEY_F1))
    {
        if (keyEnable == true)
            GameRenderer::UpdateWireframeMode();  
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void GameLogic::updateSpeed()
{
    if (WindowManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
        camera.setSpeed(20);
    else
        camera.setSpeed(10); // todo: set to 1 for evaluation
}

void GameLogic::updateInfoMode()
{
    static bool keyEnable = true;

    if (WindowManager::IsKeyPressed(GLFW_KEY_F3))
    {
        if (keyEnable == true)
            GameRenderer::UpdateInfoMode();
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void GameLogic::updateBlock()
{
    if (WindowManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) // left click
    {
        float cosYaw = cos(Utils::DegToRad(camera.getYaw()));
        float sinYaw = sin(Utils::DegToRad(camera.getYaw()));
        float cosPitch = cos(Utils::DegToRad(camera.getPitch()));
        float sinPitch = sin(Utils::DegToRad(camera.getPitch()));
        glm::vec3 raycast = glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch);
        raycast = raycast / static_cast<float>(sqrt(pow(raycast.x, 2) + pow(raycast.y, 2) + pow(raycast.z, 2)));

        for (int i = 0; i < RANGE_ACTION; i++)
        {
            glm::vec3 positionGet = camera.getPosition() + raycast * static_cast<float>(i); 
            int x = std::floor(positionGet.x);
            int y = std::floor(positionGet.y);
            int z = std::floor(positionGet.z);
            int chunkX = x / CHUNK_LENGTH;
            int chunkZ = z / CHUNK_LENGTH;
            if (x < 0)
                chunkX--;
            if (z < 0)
                chunkZ--;
            int playerChunkX = camera.getPosition().x / CHUNK_LENGTH;
            int playerChunkZ = camera.getPosition().z / CHUNK_LENGTH;
            if (camera.getPosition().x < 0)
                playerChunkX--;
            if (camera.getPosition().z < 0)
                playerChunkZ--;
            int arrayX = chunkX - playerChunkX + RENDER_DISTANCE;
            int arrayZ = chunkZ - playerChunkZ + RENDER_DISTANCE;
            if (arrayX >= RENDER_DISTANCE_2X || arrayZ >= RENDER_DISTANCE_2X)
                continue;
            const std::unique_ptr<ChunkRenderer> &chunk = world.getChunk(arrayX, arrayZ);
            if (chunk == NULL)
                continue;
            if (chunk->getBlock(x, y, z, true) == BlockType::AIR)
                continue;
            chunk->setBlock(x, y, z, BlockType::AIR, true);
            chunk->initMesh();
            chunk->updateRenderer();
            break;
        }
    }
    else if (WindowManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) // right click
        ;
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    (void)window;
    GameLogic::updateCameraAngle(xPos, yPos);
}

void GameLogic::updateCameraAngle(double xPos, double yPos)
{
    if (inputMode == GAME)
    {
        const float sensitivity = 0.1f;

        static float lastX = xPos;
        static float lastY = yPos;

        float xOffset;
        float yOffset;
        xOffset = (xPos - lastX) * sensitivity;
        yOffset = (lastY - yPos) * sensitivity;
        lastX = xPos;
        lastY = yPos;
        camera.addToYaw(xOffset);
        camera.addToPitch(yOffset);
        if (camera.getPitch() > 89.0f)
            camera.setPitch(89.0f);
        else if (camera.getPitch() < -89.0f)
            camera.setPitch(-89.0f);
        camera.updateDirection();
    }
}

void character_callback(GLFWwindow *window, unsigned int character)
{
    (void)window;
    ChatLogic::UpdateMessage(character);
}

const Camera &GameLogic::GetCamera()
{
    return (camera);
}

void GameLogic::UpdateWorldData()
{
    world.updateWorldData(camera.getPosition().x, camera.getPosition().z);
}

const InputMode &GameLogic::GetInputMode()
{
    return (inputMode);
}
const WorldData &GameLogic::GetWorldData()
{
    return (world);
}

void GameLogic::SetCameraPosition(const glm::vec3 &pos)
{
    camera.setPosition(pos);
}