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
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ft_vox", nullptr, nullptr);
}

void App::initVulkan()
{

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