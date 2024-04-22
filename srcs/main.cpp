#include "classes/WindowManager/WindowManager.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>
#include "globals.hpp"

int main()
{
#ifdef DEBUG_MODE
    std::cout << "enter debug mode" << std::endl;
#endif
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