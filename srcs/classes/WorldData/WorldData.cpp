#include "WorldData.hpp"
#include <iostream>
#include <memory>

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
    int seed = time(NULL);
    std::cout << "seed: " << seed << std::endl;
    srand(seed);
    std::unique_ptr<float[]> seedArray = std::make_unique<float[]>(1024 * 1024);
    for (int i = 0; i < 1024 * 1024; i++)
        seedArray[i] = static_cast<float>(rand() % 100) / 100;
    clock_t start = clock();
    perlinNoise = generateNoise2D(1024, 1024, seedArray, 9, 2);
    std::cout << "generation made in " << (clock() - start) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;

    chunks = std::make_unique<std::unique_ptr<ChunkMesh>[]>(RENDER_DISTANCE_2X * RENDER_DISTANCE_2X);

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            ChunkData chunkData =
                initChunkData((i - RENDER_DISTANCE) * CHUNK_LENGTH, (j - RENDER_DISTANCE) * CHUNK_LENGTH);
            chunks[i * RENDER_DISTANCE_2X + j] = std::make_unique<ChunkMesh>(chunkData);
            chunks[i * RENDER_DISTANCE_2X + j]->initMesh();
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
    int chunkIndex;
    int startX;

    if (playerChunkX < updatedPlayerChunkX)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkX + 1) * CHUNK_LENGTH;
        chunkIndex = (RENDER_DISTANCE_2X - 1) * RENDER_DISTANCE_2X;
    }
    else if (playerChunkX > updatedPlayerChunkX)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkX - 1) * CHUNK_LENGTH;
        chunkIndex = 0;
    }

    for (int j = 0; j < RENDER_DISTANCE_2X; j++)
    {
        ChunkData chunkData = initChunkData(startX, (updatedPlayerChunkZ + j - RENDER_DISTANCE) * CHUNK_LENGTH);
        chunks[chunkIndex + j] = std::make_unique<ChunkMesh>(chunkData);
        chunks[chunkIndex + j]->initMesh();
    }
}
void WorldData::updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ)
{
    int chunkIndex;
    int startZ;

    if (playerChunkZ < updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j + 1]);
        }
        startZ = (updatedPlayerChunkZ + 1) * CHUNK_LENGTH;
        chunkIndex = RENDER_DISTANCE_2X - 1;
    }
    else if (playerChunkZ > updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j - 1]);
        }
        startZ = (updatedPlayerChunkZ - 1) * CHUNK_LENGTH;
        chunkIndex = 0;
    }

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        ChunkData chunkData = initChunkData((updatedPlayerChunkX + i - RENDER_DISTANCE) * CHUNK_LENGTH, startZ);
        chunks[i * RENDER_DISTANCE_2X + chunkIndex] = std::make_unique<ChunkMesh>(chunkData);
        chunks[i * RENDER_DISTANCE_2X + chunkIndex]->initMesh();
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
                int height = perlinNoise[std::abs(modifierX + posX) * 1024 + std::abs(modifierZ + posZ)] * CHUNK_HEIGHT;
                // int height = 64;
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