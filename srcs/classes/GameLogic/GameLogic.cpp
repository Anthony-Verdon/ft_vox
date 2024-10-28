#include "GameLogic.hpp"
#include "../WindowManager/WindowManager.hpp"
#include "../Texture/Texture.hpp"
#include "../Time/Time.hpp"
#include "../LineClasses/LineRenderer/LineRenderer.hpp"
#include "../Utils/Utils.hpp"
#include "../SkyboxRenderer/SkyboxRenderer.hpp"
#include "../TextRenderer/TextRenderer.hpp"
#include "../ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera GameLogic::camera;
t_data GameLogic::data;
WorldData GameLogic::world;

void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void character_callback(GLFWwindow *window, unsigned int character);



void GameLogic::Init()
{
    camera.setPosition({8, 70, 8});
    data.message = "";
    data.lastMessageTimeStamp = -(CHAT_DISPLAY_TIME + CHAT_FADE_TIME + 1);
    data.infoMode = false;
    data.wireframeMode = false;
    data.inputMode = GAME;

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
            if (data.inputMode == CHAT)
            {
                data.lastMessage = data.message;
                data.message = "";
                data.lastMessageTimeStamp = Time::getTime();

                if (data.lastMessage.size() > 0 && data.lastMessage[0] == '/')
                    parseCommand();
            }

            if (data.inputMode == CHAT)
                data.inputMode = GAME;
            else if (data.inputMode == GAME)
                data.inputMode = CHAT;
        }
        keyEnable = false;
    }
    else
        keyEnable = true;

    switch (data.inputMode)
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

    // -----------------------------------------------

}


void GameLogic::updateChat()
{
    static bool keyEnable = true;

    if (WindowManager::IsKeyPressed(GLFW_KEY_BACKSPACE))
    {
        if (keyEnable == true && data.message != "")
        {
            data.message.erase(data.message.length() - 1, 1);
        }
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
        {
            data.wireframeMode = !data.wireframeMode;
            if (data.wireframeMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
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
            data.infoMode = !data.infoMode;
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



void GameLogic::parseCommand()
{
    static const std::map<std::string, void (*)(const std::vector<std::string> &)> commands = {{"/getBlock", &GameLogic::getBlockCommand}, {"/tp", &GameLogic::teleportCommand}};
    std::vector<std::string> commandSplit = Utils::splitLine(data.lastMessage);
    for (auto it = commands.begin(); it != commands.end(); it++)
    {
        if (it->first == commandSplit[0])
        {
            it->second(commandSplit);
            return;
        }
    }
    std::cout << "command unknown" << std::endl;
}

void GameLogic::getBlockCommand(const std::vector<std::string> &commandSplit)
{
    try
    {
        if (commandSplit.size() != 4)
            throw(std::runtime_error("invalid number of arguments"));
        int x = std::stoi(commandSplit[1]);
        int y = std::stoi(commandSplit[2]);
        int z = std::stoi(commandSplit[3]);
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
        int convertedX = ChunkData::convertWorldCoordIntoChunkCoords(x, chunkX);
        int convertedZ = ChunkData::convertWorldCoordIntoChunkCoords(z, chunkZ);
        if (arrayX >= RENDER_DISTANCE_2X || arrayZ >= RENDER_DISTANCE_2X)
            throw(std::runtime_error("chunk not loaded, be closer to it"));
        const std::unique_ptr<ChunkRenderer> &chunk = world.getChunk(arrayX, arrayZ);
        if (chunk == NULL)
            throw(std::runtime_error("chunk not loaded, wait a bit"));
        std::cout << "block at " << x << " " << y << " " << z;
        std::cout << " [chunk " << chunkX << " " << chunkZ << "]";
        std::cout << " [coord in chunk " << convertedX << " " << y << " " << convertedZ << "]";
        if (chunk->getBlock(glm::vec3(convertedX, y, convertedZ), false) != BlockType::AIR)
            std::cout << " is set";
        else
            std::cout << " isn't set";
        std::cout << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void GameLogic::teleportCommand(const std::vector<std::string> &commandSplit)
{
    try
    {
        if (commandSplit.size() != 4)
            throw(std::runtime_error("invalid number of arguments"));
        int x = std::stoi(commandSplit[1]);
        int y = std::stoi(commandSplit[2]);
        int z = std::stoi(commandSplit[3]);
        camera.setPosition(glm::vec3(x, y, z));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    (void)window;
    GameLogic::updateCameraAngle(xPos, yPos);
}

void GameLogic::updateCameraAngle(double xPos, double yPos)
{
    if (data.inputMode == GAME)
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
    GameLogic::updateMessage(character);
}

void GameLogic::updateMessage(unsigned int key)
{
    if (key < 256 && data.inputMode == CHAT)
    {
        data.message += key;
    }
}