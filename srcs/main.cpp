#include "../libs/glad/include/glad/glad.h"
#include "../libs/glm/glm/glm.hpp"
#include "../libs/glm/glm/gtc/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image/stb_image.h"
#include "classes/Shader/Shader.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

typedef struct s_camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 frontDirection = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f; // x-axis by euler
    float pitch = 0.0f; // y-axis by euler
    float roll = 0.0f;  // z-axis by euler

} t_camera;
void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    const float sensitivity = 0.1f;

    static float lastX = xPos;
    static float lastY = yPos;

    float xOffset;
    float yOffset;
    t_camera *camera = reinterpret_cast<t_camera *>(glfwGetWindowUserPointer(window));
    xOffset = (xPos - lastX) * sensitivity;
    yOffset = (lastY - yPos) * sensitivity;
    lastX = xPos;
    lastY = yPos;
    camera->yaw += xOffset;
    camera->pitch += yOffset;
    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    else if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;
}

int main()
{
    try
    {
        if (glfwInit() == GL_FALSE)
            throw(std::runtime_error("INIT_GLFW::INITIALIZATION_FAILED"));

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ft_vox", NULL, NULL);
        if (!window)
            throw(std::runtime_error("INIT_WINDOW::INITIALIZATION_FAILED"));
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw(std::runtime_error("INIT_OPENGL::INITIALIZATION_FAILED"));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        unsigned char *data = stbi_load("assets/grass.jpg", &width, &height, &nrChannels, 0);
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 4, &vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2 * 3, &faces[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        t_camera camera;
        glfwSetWindowUserPointer(window, &camera);
        glfwSetCursorPosCallback(window, mouse_callback);
        Shader shader("srcs/shaders/shader.vs", "srcs/shaders/shader.fs");
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            const float cameraSpeed = 0.05f;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera.position += cameraSpeed * camera.frontDirection;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera.position -= cameraSpeed * camera.frontDirection;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera.position -= glm::normalize(glm::cross(camera.frontDirection, camera.upDirection)) * cameraSpeed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera.position += glm::normalize(glm::cross(camera.frontDirection, camera.upDirection)) * cameraSpeed;

            glm::vec3 newDirection = glm::vec3(cosf(glm::radians(camera.yaw)) * cosf(glm::radians(camera.pitch)),
                                               sinf(glm::radians(camera.pitch)),
                                               sinf(glm::radians(camera.yaw)) * cosf(glm::radians(camera.pitch)));
            camera.frontDirection = glm::normalize(newDirection);
            shader.use();
            glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.frontDirection, camera.upDirection);
            shader.setMat4("view", view);
            glm::mat4 projection =
                glm::perspective(glm::radians(80.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
            shader.setMat4("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        stbi_image_free(data);
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return (EXIT_FAILURE);
    }
}