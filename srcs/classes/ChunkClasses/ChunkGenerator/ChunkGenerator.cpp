#include "ChunkGenerator.hpp"
#include "../../../globals.hpp"
#include <iostream>
#include <random>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>
#include <glm/gtc/noise.hpp>
#include <stb/stb_image_write.h>
unsigned long ChunkGenerator::seed = 0;
float ChunkGenerator::modifierX = 0;
float ChunkGenerator::modifierZ = 0;
int CHANNEL_NUM = 3;
void ChunkGenerator::SetSeed(unsigned long seed)
{
    ChunkGenerator::seed = seed;
    srand(seed);
    modifierX = (float)rand() / (double)RAND_MAX;
    modifierZ = (float)rand() / (double)RAND_MAX;
}

ChunkData ChunkGenerator::GenerateChunkData(int chunkX, int chunkZ)
{
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    ChunkData chunkData;
    for (int posX = 0; posX < CHUNK_LENGTH; posX++)
    {
        for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
        {
            float noiseValue = glm::simplex(glm::vec2((float)(chunkX + posX) / WORLD_LENGTH + modifierX,
                                                      (float)(chunkZ + posZ) / WORLD_LENGTH + modifierZ));

            int height;
            if (noiseValue <= 0.3)
                height = 50 + (noiseValue + 1) / 1.3 * 50;
            else if (noiseValue <= 0.4)
                height = 100 + (noiseValue - 0.3) / 0.1 * 50;
            else
                height = 150 + (noiseValue - 0.4) / 0.6 * 50;

            for (int posY = 0; posY < height; posY++)
            {
                BlockData block(chunkX + posX, posY, chunkZ + posZ, texturePattern);
                chunkData.addBlock(block);
            }
        }
    }
    return (chunkData);
}

float ChunkGenerator::getNoiseValue(int x, int z)
{
    return (glm::simplex(glm::vec2((float)x / WORLD_LENGTH + modifierX, (float)z / WORLD_LENGTH + modifierZ)));
}

/*
 * octaves = nb of layer
 * frequency = zoom
 * persistence = smooth
 */
float ChunkGenerator::getFractalNoise(float x, float z, int octaves, float frequency, float persistence)
{
    float lacunarity = 2;

    float amplitude = 2;
    float max_amplitude = 0;
    float total = 0;
    for (int i = 0; i < octaves; i++)
    {
        total += glm::simplex(glm::vec2(x, z) * frequency) * amplitude;
        max_amplitude += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return total / max_amplitude;
}
#ifdef GENERATE_MAP
#endif
void ChunkGenerator::GenerateNoiseMap()
{
    if (!std::filesystem::is_directory("imagesGenerated"))
        std::filesystem::create_directory("imagesGenerated");
    if (!std::filesystem::is_directory("imagesGenerated/" + std::to_string(seed)))
        std::filesystem::create_directory("imagesGenerated/" + std::to_string(seed));

    static const int mapLength = 256;
    std::unique_ptr<unsigned char[]> noiseMap = std::make_unique<unsigned char[]>(mapLength * mapLength * CHANNEL_NUM);
    for (int x = 0; x < mapLength; x++)
    {
        for (int z = 0; z < mapLength; z++)
        {
            float value =
                (getFractalNoise((float)(x) / mapLength + modifierX, (float)(z) / mapLength + modifierZ, 10, 4, 0.5) +
                 1) /
                2 * 256;
            for (int c = 0; c < CHANNEL_NUM; c++)
                noiseMap[(x * mapLength + z) * CHANNEL_NUM + c] = value;
        }
    }
    std::ostringstream filename;
    filename << "imagesGenerated/" << std::to_string(seed) << "/noiseMap.png";
    stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, noiseMap.get(), mapLength * CHANNEL_NUM);
}