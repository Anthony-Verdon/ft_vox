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

        while (!glfwWindowShouldClose(window))
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
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