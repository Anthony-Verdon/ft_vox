#include "WindowManager.hpp"
#include "../../globals.hpp"
#include "../Shader/Shader.hpp"
#include "../GameLogic/GameLogic.hpp"
#include "../Time/Time.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"
#include <GLFW/glfw3.h>
#include <cctype>
#include <ctime>
#include "../Texture/Texture.hpp"
#include "../GameLogic/GameLogic.hpp"
#include "../GameRenderer/GameRenderer.hpp"
#include "../Time/Time.hpp"
#include "../LineClasses/LineRenderer/LineRenderer.hpp"
#include "../Utils/Utils.hpp"
#include "../SkyboxRenderer/SkyboxRenderer.hpp"
#include "../TextRenderer/TextRenderer.hpp"
#include "../ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

GLFWwindow *WindowManager::window = NULL;

void WindowManager::InitWindow(const std::string &name, unsigned int width, unsigned int height)
{
    if (glfwInit() == GL_FALSE)
        throw(std::runtime_error("INIT_GLFW::INITIALIZATION_FAILED"));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window)
        throw(std::runtime_error("INIT_WINDOW::INITIALIZATION_FAILED"));
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw(std::runtime_error("INIT_OPENGL::INITIALIZATION_FAILED"));

    int viewPortWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewPortWidth, &viewportHeight);
    glViewport(0, 0, viewPortWidth, viewportHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // cull face enabled make openGL draw only on one side
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
}

void WindowManager::DestructWindowManager()
{
    glfwTerminate();
}

void WindowManager::StartUpdateLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        Time::updateTime();

        /* process input*/
        glfwPollEvents();
        GameLogic::ProcessInput();
   
        /* update */
        GameLogic::UpdateWorldData();

        /* rendering */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GameRenderer::Render();
   
        glfwSwapBuffers(window);
    }
}

void WindowManager::StopUpdateLoop()
{
    glfwSetWindowShouldClose(window, true);
}

bool WindowManager::IsKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

bool WindowManager::IsMouseButtonPressed(int mouseButton)
{
    return (glfwGetMouseButton(window, mouseButton) == GLFW_PRESS);
}

void WindowManager::SetCursorPosCallback(void (*func)(GLFWwindow *window, double xPos, double yPos))
{
    glfwSetCursorPosCallback(window, func);
}

void WindowManager::SetCharCallback(void (*func)(GLFWwindow *window, unsigned int character))
{
    glfwSetCharCallback(window, func);
}