#include "ChunkGenerator.hpp"
#include "../../../globals.hpp"
#include <iostream>
#include <random>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>
#include <glm/gtc/noise.hpp>
#include <map>
#include <stb/stb_image_write.h>

unsigned long ChunkGenerator::seed = 0;
float ChunkGenerator::modifierX = 0;
float ChunkGenerator::modifierY = 0;
float ChunkGenerator::modifierZ = 0;
int CHANNEL_NUM = 3;

void ChunkGenerator::SetSeed(unsigned long seed)
{
    ChunkGenerator::seed = seed;
    srand(seed);
    modifierX = (float)rand() / (double)RAND_MAX;
    modifierY = (float)rand() / (double)RAND_MAX;
    modifierZ = (float)rand() / (double)RAND_MAX;
}

unsigned long ChunkGenerator::GetSeed()
{
    return seed;
}

ChunkData ChunkGenerator::GenerateChunkData(int chunkX, int chunkZ)
{
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternBlock;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePatternBlock[i] = {0, 1}; // side
    texturePatternBlock[2] = {1, 1};     // top
    texturePatternBlock[3] = {0, 0};     // bottom

    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternWater;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePatternWater[i] = {1, 0}; // side

    int tmpNoiseSize = 128;
    int squashingFactor = 3;
    int baseTerrainEvelation = 118;
    ChunkData chunkData(chunkX / CHUNK_LENGTH, chunkZ / CHUNK_LENGTH);
    for (int posX = -1; posX < CHUNK_LENGTH_PLUS_2 - 1; posX++)
    {
        float x = (float)(chunkX + posX) / tmpNoiseSize + modifierX;
        for (int posY = 0; posY < CHUNK_HEIGHT; posY++)
        {
            //@todo y value could be calculated one time and accessed via an array after. Bias also maybe, depending of
            // the calculus
            float y = (float)(posY + CHUNK_HEIGHT / 2 - baseTerrainEvelation) / tmpNoiseSize + modifierY;
            float bias =
                cos((float)(posY + CHUNK_HEIGHT / 2 - baseTerrainEvelation) / CHUNK_HEIGHT * M_PI) * squashingFactor;
            for (int posZ = -1; posZ < CHUNK_LENGTH_PLUS_2 - 1; posZ++)
            {
                float z = (float)(chunkZ + posZ) / tmpNoiseSize + modifierZ;
                float density = glm::simplex(glm::vec3(x, y, z)) + bias;
                if (density >= 0)
                {
                    BlockData block(chunkX + posX, posY, chunkZ + posZ, texturePatternBlock);
                    chunkData.addBlock(block);
                }
                else if (density < 0 && posY <= WATER_LEVEL)
                {
                    BlockData block(chunkX + posX, posY, chunkZ + posZ, texturePatternWater);
                    chunkData.addBlock(block);
                }
            }
        }
    }
    return (chunkData);
}