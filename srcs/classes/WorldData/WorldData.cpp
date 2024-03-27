#include "WorldData.hpp"
#include <array>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <memory>
#include <utility>

std::unique_ptr<float[]> generateNoise2D(int width, int height, const std::unique_ptr<float[]> &seedArray,
                                         int nbOctaves, float bias)
{
    std::unique_ptr<float[]> noise = std::make_unique<float[]>(width * height);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            float scale = 1;
            float scaleSum = 0;
            float pitch = 0;
            for (int o = 0; o < nbOctaves; o++)
            {
                int octave = width >> o;
                int sampleX1 = (x / octave) * octave;
                int sampleY1 = (y / octave) * octave;

                int sampleX2 = (sampleX1 + octave) % width;
                int sampleY2 = (sampleY1 + octave) % height;

                float blendX = (float)(x - sampleX1) / (float)octave;
                float blendY = (float)(y - sampleY1) / (float)octave;

                float sampleT = (1.0f - blendX) * seedArray[sampleY1 * width + sampleX1] +
                                blendX * seedArray[sampleY1 * width + sampleX2];
                float sampleB = (1.0f - blendX) * seedArray[sampleY2 * width + sampleX1] +
                                blendX * seedArray[sampleY2 * width + sampleX2];

                pitch += (blendY * (sampleB - sampleT) + sampleT) * scale;
                scaleSum += scale;
                scale = scale / bias;
            }
            noise[x * height + y] = pitch / scaleSum;
        }
    }
    return (noise);
}

WorldData::WorldData()
{
    const int perlinSize = 1024;
    const int nbOctaves = 9;
    const float bias = 1.6;
    int seed = time(NULL);
    std::cout << "seed: " << seed << std::endl;
    srand(seed);
    std::unique_ptr<float[]> seedArray = std::make_unique<float[]>(perlinSize * perlinSize);
    for (int i = 0; i < perlinSize * perlinSize; i++)
        seedArray[i] = static_cast<float>(rand() % 100) / 100;
    perlinNoise = generateNoise2D(perlinSize, perlinSize, seedArray, nbOctaves, bias);
    chunks = std::make_unique<std::unique_ptr<ChunkMesh>[]>(RENDER_DISTANCE_2X * RENDER_DISTANCE_2X);

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            ChunkData chunkData =
                initChunkData((i - RENDER_DISTANCE) * CHUNK_LENGTH, (j - RENDER_DISTANCE) * CHUNK_LENGTH);
            chunks[i * RENDER_DISTANCE_2X + j] = std::make_unique<ChunkMesh>(chunkData);
        }
    }
    const std::array<int, 2> modifiers = {-1, 1};
    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            std::array<std::optional<ChunkData>, 4> neighborsChunks;
            for (int k = 0; k < 2; k++)
            {
                if (i + modifiers[k] >= 0 && i + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 0] = *(chunks[(i + modifiers[k]) * RENDER_DISTANCE_2X + j]);
                if (j + modifiers[k] >= 0 && j + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 2] = *(chunks[i * RENDER_DISTANCE_2X + j + modifiers[k]]);
            }
            chunks[i * RENDER_DISTANCE_2X + j]->initMesh(neighborsChunks);
        }
    }
}

WorldData::WorldData(const WorldData &instance)
{
    *this = instance;
}

WorldData &WorldData::operator=(const WorldData &instance)
{
    if (this != &instance)
    {
        for (int x = 0; x < RENDER_DISTANCE_2X; x++)
        {
            for (int y = 0; y < RENDER_DISTANCE_2X; y++)
            {
                ChunkMesh chunkMesh(*(instance.getChunk(x, y).get()));
                chunks[x * RENDER_DISTANCE_2X + y] = std::make_unique<ChunkMesh>(chunkMesh);
            }
        }
    }
    return (*this);
}

WorldData::~WorldData()
{
}

void WorldData::updateChunksLoad(float x, float z)
{
    if (x < 0)
        x = x - 16;
    if (z < 0)
        z = z - 16;
    static int playerChunkX = static_cast<int>(x) / CHUNK_LENGTH;
    static int playerChunkZ = static_cast<int>(z) / CHUNK_LENGTH;

    int updatedPlayerChunkX = static_cast<int>(x) / CHUNK_LENGTH;
    int updatedPlayerChunkZ = static_cast<int>(z) / CHUNK_LENGTH;
    if (playerChunkX == updatedPlayerChunkX && playerChunkZ == updatedPlayerChunkZ)
        return;

    if (playerChunkX != updatedPlayerChunkX)
        updateChunkAxisX(playerChunkX, updatedPlayerChunkX, updatedPlayerChunkZ);

    if (playerChunkZ != updatedPlayerChunkZ)
        updateChunkAxisZ(updatedPlayerChunkX, playerChunkZ, updatedPlayerChunkZ);

    playerChunkX = updatedPlayerChunkX;
    playerChunkZ = updatedPlayerChunkZ;
}

void WorldData::updateChunkAxisX(int playerChunkX, int updatedPlayerChunkX, int updatedPlayerChunkZ)
{
    std::array<int, 3> chunkToUpdate;
    int startX;
    if (playerChunkX < updatedPlayerChunkX)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkX + RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkToUpdate[0] = RENDER_DISTANCE_2X - 1;
        chunkToUpdate[1] = RENDER_DISTANCE_2X - 2;
        chunkToUpdate[2] = 0;
    }
    else if (playerChunkX > updatedPlayerChunkX)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkX - RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkToUpdate[0] = 0;
        chunkToUpdate[1] = 1;
        chunkToUpdate[2] = RENDER_DISTANCE_2X - 1;
    }

    for (int j = 0; j < RENDER_DISTANCE_2X; j++)
    {
        ChunkData chunkData = initChunkData(startX, (updatedPlayerChunkZ + j - RENDER_DISTANCE) * CHUNK_LENGTH);
        chunks[chunkToUpdate[0] * RENDER_DISTANCE_2X + j] = std::make_unique<ChunkMesh>(chunkData);
    }
    const std::array<int, 2> modifiers = {-1, 1};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            std::array<std::optional<ChunkData>, 4> neighborsChunks;
            for (int k = 0; k < 2; k++)
            {
                if (chunkToUpdate[i] + modifiers[k] >= 0 && chunkToUpdate[i] + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 0] = *(chunks[(chunkToUpdate[i] + modifiers[k]) * RENDER_DISTANCE_2X + j]);
                if (j + modifiers[k] >= 0 && j + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 2] = *(chunks[chunkToUpdate[i] * RENDER_DISTANCE_2X + j + modifiers[k]]);
            }
            chunks[chunkToUpdate[i] * RENDER_DISTANCE_2X + j]->initMesh(neighborsChunks);
        }
    }
}

void WorldData::updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ)
{
    std::array<int, 3> chunkToUpdate;
    int startZ;
    if (playerChunkZ < updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j + 1]);
        }
        startZ = (updatedPlayerChunkZ + RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkToUpdate[0] = RENDER_DISTANCE_2X - 1;
        chunkToUpdate[1] = RENDER_DISTANCE_2X - 2;
        chunkToUpdate[2] = 0;
    }
    else if (playerChunkZ > updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j - 1]);
        }
        startZ = (updatedPlayerChunkZ - RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkToUpdate[0] = 0;
        chunkToUpdate[1] = 1;
        chunkToUpdate[2] = RENDER_DISTANCE_2X - 1;
    }

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        ChunkData chunkData = initChunkData((updatedPlayerChunkX + i - RENDER_DISTANCE) * CHUNK_LENGTH, startZ);
        chunks[i * RENDER_DISTANCE_2X + chunkToUpdate[0]] = std::make_unique<ChunkMesh>(chunkData);
    }

    const std::array<int, 2> modifiers = {-1, 1};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            std::array<std::optional<ChunkData>, 4> neighborsChunks;
            for (int k = 0; k < 2; k++)
            {
                if (j + modifiers[k] >= 0 && j + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 0] = *(chunks[(j + modifiers[k]) * RENDER_DISTANCE_2X + chunkToUpdate[i]]);
                if (chunkToUpdate[i] + modifiers[k] >= 0 && chunkToUpdate[i] + modifiers[k] < RENDER_DISTANCE_2X)
                    neighborsChunks[k + 2] = *(chunks[j * RENDER_DISTANCE_2X + chunkToUpdate[i] + modifiers[k]]);
            }
            chunks[j * RENDER_DISTANCE_2X + chunkToUpdate[i]]->initMesh(neighborsChunks);
        }
    }
}

ChunkData WorldData::initChunkData(int modifierX, int modifierZ)
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
        {
            for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
            {
                // int height = perlinNoise[std::abs(modifierX + posX) * 1024 + std::abs(modifierZ + posZ)] *
                // CHUNK_HEIGHT;
                int height = 64;
                for (int posY = 0; posY < height; posY++)
                {
                    BlockData block(modifierX + posX, posY, modifierZ + posZ, texturePattern);
                    chunkData.addBlock(block);
                }
            }
        }
    }

    return (chunkData);
}

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}