#include "WindowManager.hpp"
#include "../../globals.hpp"
#include "../Shader/Shader.hpp"
#include "../SkyboxRenderer/SkyboxRenderer.hpp"
#include "../TextRenderer/TextRenderer.hpp"
#include "../Texture/Texture.hpp"
#include "../Time/Time.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"
#include <GLFW/glfw3.h>
#include <cctype>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void character_callback(GLFWwindow *window, unsigned int character);

WindowManager::WindowManager()
{
    start();
    TextRenderer::initTextRenderer();
    updateLoop();
}

WindowManager::~WindowManager()
{
    TextRenderer::destructTextRenderer();
    glfwTerminate();
}

void WindowManager::start()
{
    data.message = "";
    data.lastMessageTimeStamp = -(CHAT_DISPLAY_TIME + CHAT_FADE_TIME + 1);
    data.infoMode = false;
    data.wireframeMode = false;
    data.inputMode = GAME;
    if (glfwInit() == GL_FALSE)
        throw(std::runtime_error("INIT_GLFW::INITIALIZATION_FAILED"));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ft_vox", NULL, NULL);
    if (!window)
        throw(std::runtime_error("INIT_WINDOW::INITIALIZATION_FAILED"));
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetCharCallback(window, character_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw(std::runtime_error("INIT_OPENGL::INITIALIZATION_FAILED"));

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // cull face enabled make openGL draw only on one side
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
}

void WindowManager::updateLoop()
{
    WorldData world;
    camera.setPosition({0, 100 + glm::simplex(glm::vec2(0, 0)) * 20, 0});
    SkyboxRenderer skybox;
    Texture grassTexture("assets/textures/tileset.jpg");
    Texture skyboxTexture("assets/textures/skybox/");
    Shader WorldShader("srcs/shaders/WorldShader/WorldShader.vs", "srcs/shaders/WorldShader/WorldShader.fs");
    Shader SkyboxShader("srcs/shaders/SkyboxShader/SkyboxShader.vs", "srcs/shaders/SkyboxShader/SkyboxShader.fs");

    while (!glfwWindowShouldClose(window))
    {
        Time::updateTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* process input*/
        processInput();

        /* update matrix for world rendering */
        glm::mat4 view = glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(),
                                     camera.getUpDirection());

        float viewDistance = (RENDER_DISTANCE + 1) * CHUNK_LENGTH;
        if (viewDistance < CHUNK_HEIGHT)
            viewDistance = CHUNK_HEIGHT;
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()),
                                                (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, viewDistance);

        /* world rendering */
        WorldShader.use();
        WorldShader.setMat4("view", view);
        WorldShader.setMat4("projection", projection);
        world.updateWorldData(camera.getPosition().x, camera.getPosition().z);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.getID());
        for (size_t x = 0; x < RENDER_DISTANCE_2X; x++)
        {
            for (size_t y = 0; y < RENDER_DISTANCE_2X; y++)
            {
                const std::unique_ptr<ChunkRenderer> &chunk = world.getChunk(x, y);
                if (!chunk)
                    continue;
                glBindVertexArray(chunk->getVAO());
                glDrawElements(GL_TRIANGLES, chunk->getFaces().size(), GL_UNSIGNED_INT, 0);
            }
        }

        /* skybox rendering */
        glDepthFunc(GL_LEQUAL);
        SkyboxShader.use();
        SkyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        SkyboxShader.setMat4("projection", projection);
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, Time::getTime() / DAY_DURATION_IN_SEC * glm::radians(50.0f),
                               glm::vec3(1.0f, 0.0f, 0.0f));
        SkyboxShader.setMat4("rotation", rotation);
        glBindVertexArray(skybox.getVAO());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.getID());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        /* text rendering */
        if (data.infoMode)
            renderInformations();
        renderChat();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void WindowManager::processInput()
{
    static bool keyEnable = true;
    if (isKeyPressed(GLFW_KEY_ENTER))
    {
        if (keyEnable == true)
        {
            if (data.inputMode == CHAT)
            {
                data.lastMessage = data.message;
                data.message = "";
                data.lastMessageTimeStamp = Time::getTime();
            }

            if (data.inputMode == CHAT)
                data.inputMode = GAME;
            else if (data.inputMode == GAME)
                data.inputMode = CHAT;
        }
        keyEnable = false;
    }
    else
        keyEnable = true;

    switch (data.inputMode)
    {
    case GAME: {
        if (isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        updateWireframeMode();
        updateSpeed();
        updateInfoMode();
        int frontAxis = isKeyPressed(GLFW_KEY_W) - isKeyPressed(GLFW_KEY_S);
        int rightAxis = isKeyPressed(GLFW_KEY_D) - isKeyPressed(GLFW_KEY_A);
        int upAxis = isKeyPressed(GLFW_KEY_SPACE) - isKeyPressed(GLFW_KEY_LEFT_SHIFT);
        camera.updatePosition(frontAxis, rightAxis, upAxis);
        break;
    }
    case CHAT:
        updateChat();
        break;
    default:
        break;
    }
}

bool WindowManager::isKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

void WindowManager::updateChat()
{
    static bool keyEnable = true;

    if (isKeyPressed(GLFW_KEY_BACKSPACE))
    {
        if (keyEnable == true && data.message != "")
        {
            data.message.erase(data.message.length() - 1, 1);
        }
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void WindowManager::updateWireframeMode()
{
    static bool keyEnable = true;
    if (isKeyPressed(GLFW_KEY_F1))
    {
        if (keyEnable == true)
        {
            data.wireframeMode = !data.wireframeMode;
            if (data.wireframeMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void WindowManager::updateSpeed()
{

    if (isKeyPressed(GLFW_KEY_LEFT_CONTROL))
        camera.setSpeed(20);
    else
        camera.setSpeed(10); // todo: set to 1 for evaluation
}

void WindowManager::updateInfoMode()
{
    static bool keyEnable = true;

    if (isKeyPressed(GLFW_KEY_F3))
    {
        if (keyEnable == true)
            data.infoMode = !data.infoMode;
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void WindowManager::renderInformations()
{
    static glm::vec3 cameraOldPosition = camera.getPosition();
    glm::vec3 cameraNewPosition = camera.getPosition();

    const float scaling = 0.5f;
    TextRenderer::renderText("X: " + std::to_string(camera.getPosition().x), 0.0f,
                             WINDOW_HEIGHT - 1 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("Y: " + std::to_string(camera.getPosition().y), 0.0f,
                             WINDOW_HEIGHT - 2 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("Z: " + std::to_string(camera.getPosition().z), 0.0f,
                             WINDOW_HEIGHT - 3 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("FPS: " + std::to_string(static_cast<int>(std::round(1.0f / Time::getDeltaTime()))), 0.0f,
                             WINDOW_HEIGHT - 4 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    float distanceMade =
        sqrt(pow(cameraNewPosition.x - cameraOldPosition.x, 2) + pow(cameraNewPosition.y - cameraOldPosition.y, 2) +
             pow(cameraNewPosition.z - cameraOldPosition.z, 2));
    TextRenderer::renderText("speed : " + std::to_string(distanceMade / Time::getDeltaTime()) + " blocks per second",
                             0.0f, WINDOW_HEIGHT - 5 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));

    cameraOldPosition = cameraNewPosition;
}

void WindowManager::renderChat()
{
    const float scaling = 0.5f;
    if (data.inputMode == CHAT)
        TextRenderer::renderText("message : " + data.message, 0.0f,
                                 WINDOW_HEIGHT - 10 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                                 glm::vec4(1, 1, 1, 1));
    else if (data.inputMode == GAME)
    {
        if (Time::getTime() - data.lastMessageTimeStamp < CHAT_DISPLAY_TIME + CHAT_FADE_TIME)
        {
            float fading;
            if (Time::getTime() - data.lastMessageTimeStamp < CHAT_DISPLAY_TIME)
                fading = 1;
            else
                fading = 1 - (((Time::getTime() - data.lastMessageTimeStamp) - CHAT_DISPLAY_TIME) / CHAT_FADE_TIME);
            TextRenderer::renderText("last message : " + data.lastMessage, 0.0f,
                                     WINDOW_HEIGHT - 10 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                                     glm::vec4(1, 1, 1, fading));
        }
    }
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    WindowManager *windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
    windowManager->updateCameraAngle(xPos, yPos);
}

void WindowManager::updateCameraAngle(double xPos, double yPos)
{
    if (data.inputMode == GAME)
    {
        const float sensitivity = 0.1f;

        static float lastX = xPos;
        static float lastY = yPos;

        float xOffset;
        float yOffset;
        xOffset = (xPos - lastX) * sensitivity;
        yOffset = (lastY - yPos) * sensitivity;
        lastX = xPos;
        lastY = yPos;
        camera.addToYaw(xOffset);
        camera.addToPitch(yOffset);
        if (camera.getPitch() > 89.0f)
            camera.setPitch(89.0f);
        else if (camera.getPitch() < -89.0f)
            camera.setPitch(-89.0f);
        camera.updateDirection();
    }
}

void character_callback(GLFWwindow *window, unsigned int character)
{
    WindowManager *windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(window));
    windowManager->updateMessage(character);
}

void WindowManager::updateMessage(unsigned int key)
{
    if (key < 256 && data.inputMode == CHAT)
    {
        data.message += key;
    }
}