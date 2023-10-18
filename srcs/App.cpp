#include "App.hpp"

App::App()
{
}

App::App(const App &instance)
{
    *this = instance;
}

App &App::operator=(const App &instance)
{
    (void)instance;
    
    if (this != &instance)
    {

    }
    return (*this);
}

App::~App()
{
}

void App::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void App::initWindow()
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* description;
        glfwGetError(&description);
        std::cout << description << std::endl;
        throw std::runtime_error("glfw init failed");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ft_vox", nullptr, nullptr);
    if (!window)
        throw std::runtime_error("create window failed");
}

void App::listVulkanExtension()
{
    unsigned int extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "extensions: " << std::endl;
    for (const auto &extension: extensions)
        std::cout << extension.extensionName << std::endl;
}

void App::initVulkan()
{
    instance = std::make_unique<VkInstanceWrapper>();
    debugMessenger = std::make_unique<VkDebugMessengerWrapper>(instance->getInstance());
    physicalDevice = std::make_unique<VkPhysicalDeviceWrapper>(instance->getInstance());
    logicalDevice = std::make_unique<VkLogicalDeviceWrapper>(physicalDevice->getPhysicalDevice(), graphicsQueue);
}

void App::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void App::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}