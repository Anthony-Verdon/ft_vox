#include "../libs/glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

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
            0.0, 0.0, 0.0, //
            1.0, 0.0, 0.0, //
            0.0, 1.0, 0.0, //
            1.0, 1.0, 0.0, //
        };

        unsigned int faces[] = {
            1, 2, 3, //
            1, 2, 4  //
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4, &vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2 * 3, &faces[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
        glEnableVertexAttribArray(0);

        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return (EXIT_FAILURE);
    }
}