#include "WorldData.hpp"
#include <iostream>
#include <memory>

WorldData::WorldData()
{
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

    std::cout << "player chunk: " << updatedPlayerChunkX << " " << updatedPlayerChunkZ << std::endl;
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
        startX = (updatedPlayerChunkX + RENDER_DISTANCE_2X - 1) * CHUNK_LENGTH;
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
        startZ = (updatedPlayerChunkZ + RENDER_DISTANCE_2X - 1) * CHUNK_LENGTH;
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
        for (int posY = 0; posY < 1; posY++)
        {
            for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
            {
                BlockData block(modifierX + posX, posY, modifierZ + posZ, texturePattern);
                chunkData.addBlock(block);
            }
        }
    }
    return (chunkData);
}

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}