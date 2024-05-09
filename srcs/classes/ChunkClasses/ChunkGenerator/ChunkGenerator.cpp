#include "ChunkGenerator.hpp"
#include "../../../globals.hpp"
#include <random>
unsigned long ChunkGenerator::seed = 0;

void ChunkGenerator::SetSeed(unsigned long seed)
{
    ChunkGenerator::seed = seed;
}

ChunkData ChunkGenerator::GenerateChunkData(int chunkX, int chunkZ)
{
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    srand(seed + chunkX * WORLD_LENGTH + chunkZ);
    int height = rand() % 256;
    ChunkData chunkData;
    for (int posX = 0; posX < CHUNK_LENGTH; posX++)
    {
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
    }

    return (chunkData);
}