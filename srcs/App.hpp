#ifndef APP_HPP
#define APP_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <map>
#include <optional>
#include <memory>

#include "classes/VkInstance/VkInstanceWrapper.hpp"
#include "classes/VkDebugMessenger/VkDebugMessengerWrapper.hpp"
#include "classes/VkPhysicalDevice/VkPhysicalDeviceWrapper.hpp"
#include "classes/VkLogicalDevice/VkLogicalDeviceWrapper.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const std::vector<const char *>  validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef DEBUG
    constexpr const bool enableValidationLayers = true;
#else
    constexpr const bool enableValidationLayers = false;
#endif

class App
{
    private:
        void initWindow();
        void listVulkanExtension();
        void initVulkan();
        void mainLoop();
        void cleanup();

        GLFWwindow *window;

        std::unique_ptr<VkInstanceWrapper> instance;
        std::unique_ptr<VkDebugMessengerWrapper> debugMessenger;
        std::unique_ptr<VkPhysicalDeviceWrapper> physicalDevice;
        std::unique_ptr<VkLogicalDeviceWrapper> logicalDevice;
        VkQueue graphicsQueue;

    public:
        App();
        App(const App &instance);
        App &operator=(const App &instance);
        ~App();
        void run();
};

#endif