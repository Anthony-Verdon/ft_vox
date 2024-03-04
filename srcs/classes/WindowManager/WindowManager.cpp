#include "WindowManager.hpp"
#include "../../../libs/glm/glm/gtc/matrix_transform.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"
#include "../Time/Time.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

void mouse_callback(GLFWwindow *window, double xPos, double yPos);

WindowManager::WindowManager()
{
    start();
    updateLoop();
}

WindowManager::~WindowManager()
{
}

void WindowManager::start()
{
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
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw(std::runtime_error("INIT_OPENGL::INITIALIZATION_FAILED"));

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void WindowManager::updateLoop()
{
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    float vertices[] = {
        -0.5, -0.5, 0, 0, 0, //
        0.5,  -0.5, 0, 1, 0, //
        -0.5, 0.5,  0, 0, 1, //
        0.5,  0.5,  0, 1, 1  //
    };

    unsigned int faces[] = {
        0, 1, 2, //
        1, 2, 3  //
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 4, &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2 * 3, &faces[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Texture grassTexture("assets/grass.jpg");
    Shader shader("srcs/shaders/shader.vs", "srcs/shaders/shader.fs");

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        int frontAxis = isKeyPressed(GLFW_KEY_W) - isKeyPressed(GLFW_KEY_S);
        int rightAxis = isKeyPressed(GLFW_KEY_D) - isKeyPressed(GLFW_KEY_A);
        int upAxis = isKeyPressed(GLFW_KEY_SPACE) - isKeyPressed(GLFW_KEY_LEFT_SHIFT);
        camera.updatePosition(frontAxis, rightAxis, upAxis);

        shader.use();
        glm::mat4 view = glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(),
                                     camera.getUpDirection());
        shader.setMat4("view", view);
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.getFOV()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.getID());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

        Time::updateTime();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool WindowManager::isKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    const float sensitivity = 0.1f;

    static float lastX = xPos;
    static float lastY = yPos;

    float xOffset;
    float yOffset;
    Camera *camera = reinterpret_cast<Camera *>(glfwGetWindowUserPointer(window));
    xOffset = (xPos - lastX) * sensitivity;
    yOffset = (lastY - yPos) * sensitivity;
    lastX = xPos;
    lastY = yPos;
    camera->addToYaw(xOffset);
    camera->addToPitch(yOffset);
    if (camera->getPitch() > 89.0f)
        camera->setPitch(89.0f);
    else if (camera->getPitch() < -89.0f)
        camera->setPitch(-89.0f);
    camera->updateDirection();
}