#include "ChatLogic.hpp"
#include "../GameLogic/GameLogic.hpp"
#include "../Time/Time.hpp"
#include "../Utils/Utils.hpp"
#include "../../globals.hpp"
#include <map>
#include <iostream>

std::string ChatLogic::message = "";
std::string ChatLogic::lastMessage = "";
float ChatLogic::lastMessageTimeStamp = -(CHAT_DISPLAY_TIME + CHAT_FADE_TIME + 1);

void ChatLogic::UpdateMessage(unsigned int key)
{
    if (key >= 0 && key < 256 && GameLogic::GetInputMode() == CHAT)
    {
        message += key;
    }
}

void ChatLogic::EraseLastCharacter()
{
    if (message != "")
        message.erase(message.length() - 1, 1);
}

void ChatLogic::SendMessage()
{
    lastMessage = message;
    message = "";
    lastMessageTimeStamp = Time::getTime();

    if (lastMessage.size() > 0 && lastMessage[0] == '/')
        ParseCommand();
}

void ChatLogic::ParseCommand()
{
    static const std::map<std::string, void (*)(const std::vector<std::string> &)> commands = {{"/getBlock", &ChatLogic::GetBlockCommand}, {"/tp", &ChatLogic::TeleportCommand}};
    std::vector<std::string> commandSplit = Utils::splitLine(lastMessage);
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

void ChatLogic::GetBlockCommand(const std::vector<std::string> &commandSplit)
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
        int playerChunkX = GameLogic::GetCamera().getPosition().x / CHUNK_LENGTH;
        int playerChunkZ = GameLogic::GetCamera().getPosition().z / CHUNK_LENGTH;
        if (GameLogic::GetCamera().getPosition().x < 0)
            playerChunkX--;
        if (GameLogic::GetCamera().getPosition().z < 0)
            playerChunkZ--;
        int arrayX = chunkX - playerChunkX + RENDER_DISTANCE;
        int arrayZ = chunkZ - playerChunkZ + RENDER_DISTANCE;
        int convertedX = ChunkData::convertWorldCoordIntoChunkCoords(x, chunkX);
        int convertedZ = ChunkData::convertWorldCoordIntoChunkCoords(z, chunkZ);
        if (arrayX >= RENDER_DISTANCE_2X || arrayZ >= RENDER_DISTANCE_2X)
            throw(std::runtime_error("chunk not loaded, be closer to it"));
        const std::unique_ptr<ChunkRenderer> &chunk = GameLogic::GetWorldData().getChunk(arrayX, arrayZ);
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

void ChatLogic::TeleportCommand(const std::vector<std::string> &commandSplit)
{
    try
    {
        if (commandSplit.size() != 4)
            throw(std::runtime_error("invalid number of arguments"));
        int x = std::stoi(commandSplit[1]);
        int y = std::stoi(commandSplit[2]);
        int z = std::stoi(commandSplit[3]);
        GameLogic::SetCameraPosition(glm::vec3(x, y, z));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

std::string ChatLogic::GetMessage()
{
    return (message);
}

std::string ChatLogic::GetLastMessage()
{
    return (lastMessage);
}

float ChatLogic::GetLastMessageTimeStamp()
{
    return (lastMessageTimeStamp);
}