#ifndef APP_HPP
#define APP_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const std::vector<const char *>  validationLayers = {
    "VK_LAYER_KRONOS_validation"
};

#ifdef DEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif
class App
{
    private:
        void initWindow();
        void listVulkanExtension();
        bool checkValidationLayerSupport();
        void createInstance();
        void initVulkan();
        void mainLoop();
        void cleanup();

        GLFWwindow *window;
        VkInstance instance;
    public:
        App();
        App(const App &instance);
        App &operator=(const App &instance);
        ~App();
        void run();
};

#endif