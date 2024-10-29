#pragma once

#include <string>
#include <vector>

class ChatLogic
{
    private:
        static std::string message;
        static std::string lastMessage;
        static float lastMessageTimeStamp;

        static void ParseCommand();
        static void GetBlockCommand(const std::vector<std::string> &commandSplit);
        static void TeleportCommand(const std::vector<std::string> &commandSplit);
    
    public:
        static void UpdateMessage(unsigned int key);
        static void EraseLastCharacter();
        static void SendMessage();

        static std::string GetMessage();
        static std::string GetLastMessage();
        static float GetLastMessageTimeStamp();
};