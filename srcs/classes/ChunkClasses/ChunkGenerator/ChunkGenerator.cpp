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

    ChunkData chunkData(glm::vec2(chunkX / CHUNK_LENGTH, chunkZ / CHUNK_LENGTH));
    // y value could be calculated one time and accessed via an array after
    for (int posX = -1; posX < CHUNK_LENGTH_PLUS_2 - 1; posX++)
    {
        float x = (float)(chunkX + posX) / WORLD_LENGTH + modifierX;
        for (int posY = 0; posY < CHUNK_HEIGHT; posY++)
        {
            float y = (float)posY / CHUNK_HEIGHT + modifierY;
            float bias = 2 * cos((float)posY / CHUNK_HEIGHT * M_PI);
            for (int posZ = -1; posZ < CHUNK_LENGTH_PLUS_2 - 1; posZ++)
            {
                float z = (float)(chunkZ + posZ) / WORLD_LENGTH + modifierZ;
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

// convert a value in [-1; 1] to [0; 1]
float ChunkGenerator::convertRange(float value)
{
    return ((value + 1) / 2);
}

float ChunkGenerator::roundValue(float value, int roundFactor)
{
    int roundedValue = value * roundFactor;
    return ((float)roundedValue / roundFactor);
}

float ChunkGenerator::getValueFromGraph(float value, eGraphType graphType)
{
    /* these map correspond to graphics,
    float is between [0; 1]
    and int is value between [0; 256]
    */
    static const std::map<float, int> continentalnessGraph = {{0, 50},     {0.1, 50},  {0.11, 85},
                                                              {0.65, 100}, {0.8, 130}, {1, 150}};
    // static const std::map<float, int> continentalnessGraph = {{0, 10},    {0.25, 10}, {0.3, 50},
    //                                                          {0.39, 50}, {0.4, 100}, {1, 120}};
    static const std::map<float, int> erosionGraph = {{0, 120}, {0.3, 100}, {0.4, 50}, {1, 50}};
    static const std::map<float, int> pvGraph = {{0, 100}, {0.2, 150}, {0.5, 200}, {0.6, 250}, {1, 250}};

    std::map<float, int> graph;

    switch (graphType)
    {
    case CONTINENTALNESS_GRAPH:
        graph = continentalnessGraph;
        break;
    case EROSION_GRAPH:
        graph = erosionGraph;
        break;
    case PV_GRAPH:
        graph = pvGraph;
        break;
    default:
        return -1;
    }

    std::map<float, int>::iterator lowerIt = graph.lower_bound(value);
    lowerIt--;
    std::map<float, int>::iterator upperIt = std::next(lowerIt);

    float returnValue = lowerIt->second + (value - lowerIt->first) / (upperIt->first - lowerIt->first) *
                                              (upperIt->second - lowerIt->second);
    return (returnValue);
}
/*
 * octaves = nb of layer
 * frequency = zoom
 * persistence = smooth
 */
float ChunkGenerator::getFractalNoise(float x, float z, int octaves, float frequency, float persistence)
{
    float lacunarity = 2;

    float amplitude = 4;
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

void ChunkGenerator::generateImage(const std::string &name, int octaves, float frequency, float persistence,
                                   int roundFactor)
{
    static const int mapLength = 256;
    std::unique_ptr<unsigned char[]> noiseMap = std::make_unique<unsigned char[]>(mapLength * mapLength * CHANNEL_NUM);
    for (int x = 0; x < mapLength; x++)
    {
        for (int z = 0; z < mapLength; z++)
        {
            float value = (getFractalNoise((float)(x) / mapLength + modifierX, (float)(z) / mapLength + modifierZ,
                                           octaves, frequency, persistence) +
                           1) /
                          2;
            int roundedValue = value * roundFactor;
            value = (float)roundedValue / roundFactor;
            value = value * 256;
            for (int c = 0; c < CHANNEL_NUM; c++)
                noiseMap[(x * mapLength + z) * CHANNEL_NUM + c] = value;
        }
    }
    std::ostringstream filename;
    filename << "imagesGenerated/" << std::to_string(seed) << "/" << name << ".png";
    stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, noiseMap.get(), mapLength * CHANNEL_NUM);
}

void ChunkGenerator::GenerateNoiseMap()
{
    if (!std::filesystem::is_directory("imagesGenerated"))
        std::filesystem::create_directory("imagesGenerated");
    if (!std::filesystem::is_directory("imagesGenerated/" + std::to_string(seed)))
        std::filesystem::create_directory("imagesGenerated/" + std::to_string(seed));

    generateImage("continentalness", CONTINENTALNESS_OCTAVES, CONTINENTALNESS_FREQUENCY, CONTINENTALNESS_PERSISTENCE,
                  CONTINENTALNESS_ROUND_FACTOR);
    generateImage("erosion", EROSION_OCTAVES, EROSION_FREQUENCY, EROSION_PERSISTENCE, EROSION_ROUND_FACTOR);
    generateImage("peakAndValleys", PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE, PV_ROUND_FACTOR);
}

#endif