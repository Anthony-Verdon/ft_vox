#include "classes/WindowManager/WindowManager.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        WindowManager windowManager;
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return (EXIT_FAILURE);
    }
}