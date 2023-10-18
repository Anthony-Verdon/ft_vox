#ifndef VK_DEBUG_MESSENGER_WRAPPER_HPP
#define VK_DEBUG_MESSENGER_WRAPPER_HPP

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>

class VkDebugMessengerWrapper
{
    private:
        VkDebugUtilsMessengerEXT debugMessenger;

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    public:
        VkDebugMessengerWrapper(VkInstance instance);
        ~VkDebugMessengerWrapper();
        VkDebugUtilsMessengerEXT getDebugMessenger();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
};

#endif