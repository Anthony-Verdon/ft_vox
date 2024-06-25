#include "classes/ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"
#include "classes/WindowManager/WindowManager.hpp"
#include "globals.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
#ifdef DEBUG_MODE
    std::cout << "enter debug  mode" << std::endl;
#endif
    try
    {
        if (argc > 2)
            throw(std::runtime_error("only one argument accepted"));

        if (argc == 1)
            ChunkGenerator::SetSeed(time(nullptr));
        else
            ChunkGenerator::SetSeed(std::stoul(argv[1]));
        std::cout << "seed: " << ChunkGenerator::GetSeed() << std::endl;

#ifdef GENERATE_MAP
        ChunkGenerator::GenerateNoiseMap();
#endif
        WindowManager windowManager;
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &exception)
    {
        std::cerr << exception.what() << std::endl;
        return (EXIT_FAILURE);
    }
}