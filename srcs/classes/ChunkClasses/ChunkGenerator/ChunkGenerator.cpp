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

void ChunkGenerator::GenerateMap()
{
    if (!std::filesystem::is_directory("imagesGenerated"))
        std::filesystem::create_directory("imagesGenerated");
    if (!std::filesystem::is_directory("imagesGenerated/" + std::to_string(seed)))
        std::filesystem::create_directory("imagesGenerated/" + std::to_string(seed));

    std::unique_ptr<unsigned char[]> mapBeforeZoom;
    std::unique_ptr<unsigned char[]> map;
    map = std::make_unique<unsigned char[]>(WORLD_LENGTH * WORLD_LENGTH * CHANNEL_NUM);
    glm::detail::permute(seed);
    for (int x = 0; x < WORLD_LENGTH; x++)
    {
        for (int z = 0; z < WORLD_LENGTH; z++)
        {
            float value = glm::simplex(glm::vec2((float)(x) / WORLD_LENGTH, (float)z / WORLD_LENGTH)) * 128 + 128;
            for (int c = 0; c < 3; c++)
                map[(x * WORLD_LENGTH + z) * CHANNEL_NUM + c] = value;
        }
    }
    std::ostringstream filename;
    filename << "simplex.png";
    stbi_write_png(filename.str().c_str(), WORLD_LENGTH, WORLD_LENGTH, CHANNEL_NUM, map.get(),
                   WORLD_LENGTH * CHANNEL_NUM);
    return;
    int scale = 4096;
    srand(seed);
    while (scale >= 2048)
    {
        int mapLength = WORLD_LENGTH / scale;
        std::cout << "zoom scale: " << scale << " | map length: " << mapLength << std::endl;

        if (scale == 4096)
        {
            size_t mapSize = mapLength * mapLength * CHANNEL_NUM;
            map = std::make_unique<unsigned char[]>(mapSize);

            for (size_t i = 0; i < mapSize; i += CHANNEL_NUM)
            {
                std::array<int, 3> color;
                int value = rand() % 10;
                if (value == 0)
                    color = GREEN_COLOR;
                else
                    color = BlUE_COLOR;

                for (int j = 0; j < 3; j++)
                    map[i + j] = color[j];
            }
        }
        else if (scale == 2048)
        {
            map = Zoom(scale, mapBeforeZoom);
            size_t mapSize = mapLength * mapLength * CHANNEL_NUM;
            std::unique_ptr<unsigned char[]> mapWithLessOcean = std::make_unique<unsigned char[]>(mapSize);
            for (size_t i = 0; i < mapSize; i += CHANNEL_NUM)
            {
                int coordX = i / CHANNEL_NUM / mapLength;
                int coordZ = i / CHANNEL_NUM % mapLength;

                std::array<int, 3> color;
                bool isSurroundedByOcean = true;

                if (coordX - 1 >= 0)
                {
                    for (int j = 0; j < 3; j++)
                        color[j] = map[(coordX - 1) * mapLength * CHANNEL_NUM + coordZ * CHANNEL_NUM + j];
                    if (color == GREEN_COLOR)
                        isSurroundedByOcean = false;
                }
                if (coordX + 1 < mapLength)
                {
                    for (int j = 0; j < 3; j++)
                        color[j] = map[(coordX + 1) * mapLength * CHANNEL_NUM + coordZ * CHANNEL_NUM + j];
                    if (color == GREEN_COLOR)
                        isSurroundedByOcean = false;
                }
                if (coordZ - 1 >= 0)
                {
                    for (int j = 0; j < 3; j++)
                        color[j] = map[coordX * mapLength * CHANNEL_NUM + (coordZ - 1) * CHANNEL_NUM + j];
                    if (color == GREEN_COLOR)
                        isSurroundedByOcean = false;
                }
                if (coordZ + 1 < mapLength)
                {
                    for (int j = 0; j < 3; j++)
                        color[j] = map[coordX * mapLength * CHANNEL_NUM + (coordZ + 1) * CHANNEL_NUM + j];
                    if (color == GREEN_COLOR)
                        isSurroundedByOcean = false;
                }

                for (int j = 0; j < 3; j++)
                    color[j] = map[i + j];
                if (isSurroundedByOcean && rand() % 2 == 0)
                    color = GREEN_COLOR;
                for (int j = 0; j < 3; j++)
                    mapWithLessOcean[i + j] = color[j];
            }
            {
                std::ostringstream filename;
                filename << "imagesGenerated/" << std::to_string(seed) << "/scale_" << scale
                         << "_BeforeRemoveOcean.png";
                stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, map.get(),
                               mapLength * CHANNEL_NUM);
            }
            map = std::move(mapWithLessOcean);
            {
                std::ostringstream filename;
                filename << "imagesGenerated/" << std::to_string(seed) << "/scale_" << scale << "_AfterRemoveOcean.png";
                stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, map.get(),
                               mapLength * CHANNEL_NUM);
            }
        }

        /*
        // add island layers => really create an automata cellular for this
        std::array<std::array<int, 3>, 8> neighbors;
        int neighborsIndex = 0;
        for (int x = -1; x <= 1; x++)
        {
            for (int z = -1; z <= 1; z++)
            {
                if (x != 0 && (coordXBeforeZoom + x) >= 0 && (coordXBeforeZoom + x) < (mapLength / 2) &&
                    z != 0 && (coordZBeforeZoom + z) >= 0 && (coordZBeforeZoom + z) < (mapLength / 2))
                {
                    for (int j = 0; j < 3; j++)
                        neighbors[neighborsIndex][j] =
                            mapBeforeZoom[(coordXBeforeZoom + x) * (mapLength / 2) * CHANNEL_NUM +
                                          (coordZBeforeZoom + z) * CHANNEL_NUM + j];
                    neighborsIndex++;
                }
            }
        }

        int nbGreenTiles = 0;
        int nbBlueTiles = 0;
        for (int j = 0; j < 8; j++)
        {
            if (neighbors[j] == GREEN_COLOR)
                nbGreenTiles++;
            else if (neighbors[j] == BlUE_COLOR)
                nbBlueTiles++;
        }

        int value = rand() % 8;
        if (value < nbGreenTiles)
        {
            if (color == GREEN_COLOR)
                color = BlUE_COLOR;
            else
                color = GREEN_COLOR;
        }
        */

        std::ostringstream filename;
        filename << "imagesGenerated/" << std::to_string(seed) << "/scale_" << scale << ".png";
        stbi_write_png(filename.str().c_str(), mapLength, mapLength, CHANNEL_NUM, map.get(), mapLength * CHANNEL_NUM);
        scale = scale >> 1; // scale / 2
        mapBeforeZoom = std::move(map);
    }
}

std::unique_ptr<unsigned char[]> ChunkGenerator::Zoom(int scale, const std::unique_ptr<unsigned char[]> &mapBeforeZoom)
{
    int mapLength = WORLD_LENGTH / scale;
    size_t mapSize = mapLength * mapLength * CHANNEL_NUM;
    std::unique_ptr<unsigned char[]> map = std::make_unique<unsigned char[]>(mapSize);

    for (size_t i = 0; i < mapSize; i += CHANNEL_NUM)
    {
        int coordX = i / CHANNEL_NUM / mapLength;
        int coordZ = i / CHANNEL_NUM % mapLength;
        int coordXBeforeZoom = coordX / 2;
        int coordZBeforeZoom = coordZ / 2;

        std::array<int, 3> color;
        // zoom and scale perfectly => need to add "errors"
        for (int j = 0; j < 3; j++)
            color[j] =
                mapBeforeZoom[coordXBeforeZoom * (mapLength / 2) * CHANNEL_NUM + coordZBeforeZoom * CHANNEL_NUM + j];

        // erosion could be better
        int value = rand() % 10;
        if (value <= 1)
        {
            if (color == GREEN_COLOR)
                color = BlUE_COLOR;
            else
                color = GREEN_COLOR;
        }

        for (int j = 0; j < 3; j++)
            map[i + j] = color[j];
    }

    return (map);
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