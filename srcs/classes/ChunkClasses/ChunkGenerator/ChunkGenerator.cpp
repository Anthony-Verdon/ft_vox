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
            float x = (float)(chunkX + posX) / WORLD_LENGTH + modifierX;
            float z = (float)(chunkZ + posZ) / WORLD_LENGTH + modifierZ;

            float continentalnessValue =
                getFractalNoise(x, z, CONTINENTALNESS_OCTAVES, CONTINENTALNESS_FREQUENCY, CONTINENTALNESS_PERSISTENCE);
            continentalnessValue = convertRange(continentalnessValue);
            continentalnessValue = getValueFromGraph(continentalnessValue, CONTINENTALNESS_GRAPH);

            float erosionValue = getFractalNoise(x, z, EROSION_OCTAVES, EROSION_FREQUENCY, EROSION_PERSISTENCE);
            erosionValue = convertRange(erosionValue);
            erosionValue = getValueFromGraph(erosionValue, EROSION_GRAPH);

            float peakAndValleysValue = getFractalNoise(x, z, PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE);
            peakAndValleysValue = convertRange(peakAndValleysValue);
            peakAndValleysValue = getValueFromGraph(peakAndValleysValue, PV_GRAPH);

            int height = continentalnessValue;
            if (height > 100)
                height = (continentalnessValue + erosionValue + peakAndValleysValue) / 3;
            for (int posY = 0; posY < height && posY < 256; posY++)
            {
                BlockData block(chunkX + posX, posY, chunkZ + posZ, texturePattern);
                chunkData.addBlock(block);
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
    static const std::map<float, int> continentalnessGraph = {{0, 10},    {0.25, 10}, {0.3, 50},
                                                              {0.39, 50}, {0.4, 100}, {1, 120}};
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