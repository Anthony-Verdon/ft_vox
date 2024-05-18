#include "ChunkGenerator.hpp"
#include "../../../globals.hpp"
#include <iostream>
#include <random>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>
#include <stb/stb_image_write.h>
unsigned long ChunkGenerator::seed = 0;

void ChunkGenerator::SetSeed(unsigned long seed)
{
    ChunkGenerator::seed = seed;
}

void ChunkGenerator::GenerateMap()
{
    if (!std::filesystem::is_directory("imagesGenerated"))
        std::filesystem::create_directory("imagesGenerated");

    std::unique_ptr<unsigned char[]> map;
    int scale = 4096;
    int CHANNEL_NUM = 3;
    while (scale >= 16)
    {
        if (!std::filesystem::is_directory("imagesGenerated/" + std::to_string(seed)))
            std::filesystem::create_directory("imagesGenerated/" + std::to_string(seed));
        srand(seed);
        std::cout << "zoom scale: " << scale << std::endl;
        int mapLength = WORLD_LENGTH / scale;
        size_t mapSize = mapLength * mapLength * CHANNEL_NUM;
        map = std::make_unique<unsigned char[]>(mapSize);
        for (size_t i = 0; i < mapSize; i += CHANNEL_NUM)
        {
            int value = rand() % 100;
            map[i + 0] = value;
            map[i + 1] = value;
            map[i + 2] = value;
        }
        std::ostringstream filename;
        filename << "imagesGenerated/" << std::to_string(seed) << "/scale_" << scale << ".png";
        stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, map.get(), mapLength * CHANNEL_NUM);
        scale = scale >> 1;
    }
}

ChunkData ChunkGenerator::GenerateChunkData(int chunkX, int chunkZ)
{
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    srand(seed + chunkX * NB_CHUNK_ON_EDGE + chunkZ);
    int height = rand() % 256;
    ChunkData chunkData;
    for (int posX = 0; posX < CHUNK_LENGTH; posX++)
    {
        for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
        {
            for (int posY = 0; posY < height; posY++)
            {
                BlockData block(chunkX + posX, posY, chunkZ + posZ, texturePattern);
                chunkData.addBlock(block);
            }
        }
    }

    return (chunkData);
}