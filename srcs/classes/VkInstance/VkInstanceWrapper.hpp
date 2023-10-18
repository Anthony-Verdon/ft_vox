#ifndef VK_INSTANCE_WRAPPER_HPP
#define VK_INSTANCE_WRAPPER_HPP

#include <vulkan/vulkan.h>
#include "../VkDebugMessenger/VkDebugMessengerWrapper.hpp"

class VkInstanceWrapper
{
private:
    bool checkValidationLayerSupport();

    VkInstance instance;

public:
    VkInstanceWrapper(/* args */);
    ~VkInstanceWrapper();
    VkInstance getInstance();
};

#endif