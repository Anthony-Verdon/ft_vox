
#include "ChunkGenerator.hpp"
#include "../../../assert.hpp"
#include "../../../globals.hpp"
#include "../../BlockClasses/BlockDico/BlockDico.hpp"
#include <iostream>
#include <random>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Utils/Utils.hpp"
#include <algorithm>
#include <filesystem>
#include <glm/gtc/noise.hpp>
#include <stb/stb_image_write.h>

unsigned long ChunkGenerator::seed = 0;
float ChunkGenerator::modifierX = 0;
float ChunkGenerator::modifierY = 0;
float ChunkGenerator::modifierZ = 0;
std::map<glm::vec2, std::vector<std::pair<glm::vec3, BlockType>>, Vec2Compare> ChunkGenerator::featuresToPlace;
std::vector<glm::vec2> ChunkGenerator::chunkGeneratedOnce;

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

ChunkData ChunkGenerator::GenerateChunkData(int chunkX, int chunkZ, bool firstIteration)
{
    if (firstIteration)
    {
        for (int x = -1; x <= 1; x++)
        {
            for (int z = -1; z <= 1; z++)
            {
                if (x == 0 && z == 0)
                    continue;
                glm::vec2 newChunkCoord = glm::vec2(chunkX + x * CHUNK_LENGTH, chunkZ + z * CHUNK_LENGTH);
                if (std::find(chunkGeneratedOnce.begin(), chunkGeneratedOnce.end(), newChunkCoord) ==
                    chunkGeneratedOnce.end())
                    GenerateChunkData(newChunkCoord.x, newChunkCoord.y, false);
            }
        }
    }

    unsigned long chunkSeed = seed + chunkX * WORLD_LENGTH / CHUNK_LENGTH + chunkZ;
    srand(chunkSeed);

    ChunkData chunkData(chunkX / CHUNK_LENGTH, chunkZ / CHUNK_LENGTH);
    GenerateTerrain(chunkData);
    if (std::find(chunkGeneratedOnce.begin(), chunkGeneratedOnce.end(), chunkData.getChunkCoord()) ==
        chunkGeneratedOnce.end())
        GenerateFeatures(chunkData);
    AddFeatures(chunkData);

    if (std::find(chunkGeneratedOnce.begin(), chunkGeneratedOnce.end(), chunkData.getChunkCoord()) ==
        chunkGeneratedOnce.end())
        chunkGeneratedOnce.push_back(chunkData.getChunkCoord());

    return (chunkData);
}

void ChunkGenerator::GenerateTerrain(ChunkData &chunkData)
{
    int chunkX = chunkData.getChunkCoordX() * CHUNK_LENGTH;
    int chunkZ = chunkData.getChunkCoordZ() * CHUNK_LENGTH;
    for (int posX = -1; posX < CHUNK_LENGTH_PLUS_2 - 1; posX++)
    {
        float x = GetNoisePosition(chunkX + posX, true);
        for (int posZ = -1; posZ < CHUNK_LENGTH_PLUS_2 - 1; posZ++)
        {
            float z = GetNoisePosition(chunkZ + posZ, false);
            int terrainHeight = 128;
            {
                float continentalnessNoiseValue = getFractalNoise(
                    x, z, CONTINENTALNESS_OCTAVES, CONTINENTALNESS_FREQUENCY, CONTINENTALNESS_PERSISTENCE);
                // { value range, height}
                std::map<float, int> continentalnessGraph = {{-1, 20}, {-0.33, WATER_LEVEL}, {0.33, 80}, {1, 150}};
                auto previousIt = continentalnessGraph.begin();
                for (auto it = continentalnessGraph.begin(); it != continentalnessGraph.end(); it++)
                {
                    if (continentalnessNoiseValue < it->first)
                    {
                        if (it != continentalnessGraph.begin())
                            terrainHeight = previousIt->second + (continentalnessNoiseValue - previousIt->first) /
                                                                     (it->first - previousIt->first) *
                                                                     (it->second - previousIt->second);
                        else
                            terrainHeight = previousIt->second;
                        break;
                    }
                    previousIt = it;
                }
            }

            if (terrainHeight >= WATER_LEVEL)
            {
                float pvNoiseValue = abs(getFractalNoise(x, z, PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE));
                terrainHeight = (WATER_LEVEL - 2) + pvNoiseValue * (terrainHeight - (WATER_LEVEL - 2));
            }

            int posY = 0;
            for (; posY <= terrainHeight; posY++)
            {
                if (posY == terrainHeight && terrainHeight >= WATER_LEVEL)
                    chunkData.setBlock(chunkX + posX, posY, chunkZ + posZ, BlockType::GRASS, true);

                else if (posY >= terrainHeight - 3)
                    chunkData.setBlock(chunkX + posX, posY, chunkZ + posZ, BlockType::DIRT, true);
                else
                    chunkData.setBlock(chunkX + posX, posY, chunkZ + posZ, BlockType::STONE, true);
            }
            for (; posY <= WATER_LEVEL; posY++)
                chunkData.setBlock(chunkX + posX, posY, chunkZ + posZ, BlockType::WATER, true);
        }
    }
}

void ChunkGenerator::GenerateFeatures(ChunkData &chunkData)
{
    glm::vec2 chunkCoord = {chunkData.getChunkCoordX() * CHUNK_LENGTH, chunkData.getChunkCoordZ() * CHUNK_LENGTH};
    for (int x = -1; x < CHUNK_LENGTH_PLUS_2 - 1; x++)
    {
        for (int z = -1; z < CHUNK_LENGTH_PLUS_2 - 1; z++)
        {
            if (x == -1 || z == -1 || x == CHUNK_LENGTH || z == CHUNK_LENGTH)
                continue;

            int y;
            for (y = CHUNK_HEIGHT - 1; y >= 0; y--)
            {
                if (chunkData.getBlock(chunkCoord.x + x, y, chunkCoord.y + z, true) != BlockType::AIR)
                    break;
            }

            if (chunkData.getBlock(chunkCoord.x + x, y, chunkCoord.y + z, true) != BlockType::GRASS &&
                chunkData.getBlock(chunkCoord.x + x, y, chunkCoord.y + z, true) != BlockType::DIRT)
                continue;

            if (rand() % 100 < 2)
                GenerateTree(chunkCoord, glm::vec3(x, y, z));
        }
    }
}

void ChunkGenerator::GenerateTree(const glm::vec2 &chunkCoord, const glm::vec3 &treeCoord)
{
    int treeHeight = 3 + rand() % 3;
    for (int i = 0; i < treeHeight; i++)
        featuresToPlace[chunkCoord].push_back(std::pair<glm::vec3, BlockType>{
            glm::vec3(chunkCoord.x + treeCoord.x, treeCoord.y + 1 + i, chunkCoord.y + treeCoord.z), BlockType::WOOD});

    for (int x = -2; x < 3; x++)
    {
        for (int y = 0; y < treeHeight; y++)
        {
            for (int z = -2; z < 3; z++)
            {
                if (x == 0 && y < treeHeight - 3 && z == 0)
                    continue;

                glm::vec3 leavesCoord = {treeCoord.x + x, treeCoord.y + 3 + y, treeCoord.z + z};
                glm::vec3 leavesWorldCoord = {chunkCoord.x + leavesCoord.x, leavesCoord.y,
                                              chunkCoord.y + leavesCoord.z};

                glm::vec2 chunkCoordKey = chunkCoord;
                if (leavesCoord.x < 0)
                    chunkCoordKey.x -= CHUNK_LENGTH;
                else if (leavesCoord.x >= CHUNK_LENGTH)
                    chunkCoordKey.x += CHUNK_LENGTH;
                if (leavesCoord.z < 0)
                    chunkCoordKey.y -= CHUNK_LENGTH;
                else if (leavesCoord.z >= CHUNK_LENGTH)
                    chunkCoordKey.y += CHUNK_LENGTH;

                featuresToPlace[chunkCoordKey].push_back({leavesWorldCoord, BlockType::LEAVES});

                int value = (int)leavesWorldCoord.x % CHUNK_LENGTH;
                if (value < 0)
                    value += CHUNK_LENGTH;
                if (value == 0)
                {
                    featuresToPlace[{chunkCoordKey.x - CHUNK_LENGTH, chunkCoordKey.y}].push_back(
                        {leavesWorldCoord, BlockType::LEAVES});
                }
                else if (value == CHUNK_LENGTH - 1)
                {
                    featuresToPlace[{chunkCoordKey.x + CHUNK_LENGTH, chunkCoordKey.y}].push_back(
                        {leavesWorldCoord, BlockType::LEAVES});
                }

                value = (int)leavesWorldCoord.z % CHUNK_LENGTH;
                if (value < 0)
                    value += CHUNK_LENGTH;
                if (value == 0)
                {
                    featuresToPlace[{chunkCoordKey.x, chunkCoordKey.y - CHUNK_LENGTH}].push_back(
                        {leavesWorldCoord, BlockType::LEAVES});
                }
                else if (value == CHUNK_LENGTH - 1)
                {
                    featuresToPlace[{chunkCoordKey.x, chunkCoordKey.y + CHUNK_LENGTH}].push_back(
                        {leavesWorldCoord, BlockType::LEAVES});
                }
            }
        }
    }
}

void ChunkGenerator::AddFeatures(ChunkData &chunkData)
{
    glm::vec2 chunkCoord = {chunkData.getChunkCoordX() * CHUNK_LENGTH, chunkData.getChunkCoordZ() * CHUNK_LENGTH};
    auto itMap = featuresToPlace.find({chunkCoord.x, chunkCoord.y});
    if (itMap == featuresToPlace.end())
        return;
    for (auto itVector = itMap->second.begin(); itVector != itMap->second.end(); itVector++)
    {
        if (chunkData.getBlock(itVector->first, true) == BlockType::AIR)
            chunkData.setBlock(itVector->first, itVector->second, true);
    }
}

float ChunkGenerator::GetNoisePosition(int pos, bool xCoord)
{
    if (xCoord)
        return (float)pos / NOISE_SIZE + modifierX;
    else
        return (float)pos / NOISE_SIZE + modifierZ;
}

// octaves = nb of layer
// frequency = zoom
// persistence = smooth
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
float ChunkGenerator::getValueFromGraph(float value, eGraphType graphType)
{
    // these map correspond to graphics,
    // float is between [0; 1]
    // and int is value between [0; 256]
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

void ChunkGenerator::generateImage(const std::string &name, int octaves, float frequency, float persistence,
                                   int roundFactor, bool drawColor, bool ridgedNoise)
{
    (void)roundFactor;
    static const int mapLength = 256;
    std::unique_ptr<unsigned char[]> noiseMap = std::make_unique<unsigned char[]>(mapLength * mapLength * CHANNEL_NUM);
    for (int x = 0; x < mapLength; x++)
    {
        for (int z = 0; z < mapLength; z++)
        {
            float value = getFractalNoise((float)(x) / mapLength + modifierX, (float)(z) / mapLength + modifierZ,
                                          octaves, frequency, persistence);
            if (ridgedNoise)
                value = abs(value);
            value = 128 + value * 128;

            if (drawColor)
            {
                if (value <= 128)
                {
                    // blue == ocean
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 0] = 0;
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 1] = 0;
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 2] = 255;
                }
                else
                {
                    // green == land
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 0] = 0;
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 1] = 255;
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + 2] = 0;
                }
            }
            else
            {
                for (int c = 0; c < CHANNEL_NUM; c++)
                    noiseMap[(x * mapLength + z) * CHANNEL_NUM + c] = value;
            }
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
                  CONTINENTALNESS_ROUND_FACTOR, false, false);
    generateImage("continentalness_color", CONTINENTALNESS_OCTAVES, CONTINENTALNESS_FREQUENCY,
                  CONTINENTALNESS_PERSISTENCE, CONTINENTALNESS_ROUND_FACTOR, true, false);
    generateImage("erosion", EROSION_OCTAVES, EROSION_FREQUENCY, EROSION_PERSISTENCE, EROSION_ROUND_FACTOR, false,
                  false);
    generateImage("peakAndValleys", PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE, PV_ROUND_FACTOR, false, true);
    generateImage("peakAndValleys_color", PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE, PV_ROUND_FACTOR, true, true);
}
#endif