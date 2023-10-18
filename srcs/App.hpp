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
        bool checkValidationLayerSupport();
        void createInstance();
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        void pickPhysicalDevice();
        int rateDeviceSuitability(VkPhysicalDevice device);
        void createLogicalDevice();
        void initVulkan();
        void mainLoop();
        void cleanup();

        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
        VkQueue graphicsQueue;

    public:
        App();
        App(const App &instance);
        App &operator=(const App &instance);
        ~App();
        void run();
};

#endif