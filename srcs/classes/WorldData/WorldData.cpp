#include "WorldData.hpp"
#include <iostream>
#include <memory>

WorldData::WorldData()
{
    chunks = std::make_unique<std::unique_ptr<ChunkMesh>[]>(RENDER_DISTANCE * RENDER_DISTANCE);

    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    for (int i = 0; i < RENDER_DISTANCE; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE; j++)
        {
            ChunkData chunkData;
            for (int x = 0; x < CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    for (int z = 0; z < CHUNK_LENGTH; z++)
                    {
                        int posX = x + i * CHUNK_LENGTH;
                        int posZ = z + j * CHUNK_LENGTH;
                        BlockData block(posX, y, posZ, texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            ChunkMesh chunkMesh(chunkData);
            chunkMesh.initMesh();
            chunks[i * RENDER_DISTANCE + j] = std::make_unique<ChunkMesh>(chunkMesh);
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
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            for (int y = 0; y < RENDER_DISTANCE; y++)
            {
                ChunkMesh chunkMesh(*(instance.getChunk(x, y).get()));
                chunks[x * RENDER_DISTANCE + y] = std::make_unique<ChunkMesh>(chunkMesh);
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
        for (int i = 0; i < RENDER_DISTANCE - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[(i + 1) * RENDER_DISTANCE + j]);
        }
        startX = (updatedPlayerChunkX + RENDER_DISTANCE - 1) * CHUNK_LENGTH;
        chunkIndex = (RENDER_DISTANCE - 1) * RENDER_DISTANCE;
    }
    else if (playerChunkX > updatedPlayerChunkX)
    {
        for (int i = RENDER_DISTANCE - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[(i - 1) * RENDER_DISTANCE + j]);
        }
        startX = (updatedPlayerChunkX - 1) * CHUNK_LENGTH;
        chunkIndex = 0;
    }

    for (int j = 0; j < RENDER_DISTANCE; j++)
    {
        ChunkData chunkData = initChunkData(startX, (updatedPlayerChunkZ + j) * CHUNK_LENGTH);
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
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE - 1; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[i * RENDER_DISTANCE + j + 1]);
        }
        startZ = (updatedPlayerChunkZ + RENDER_DISTANCE - 1) * CHUNK_LENGTH;
        chunkIndex = RENDER_DISTANCE - 1;
    }
    else if (playerChunkZ > updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            for (int j = RENDER_DISTANCE - 1; j >= 1; j--)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[i * RENDER_DISTANCE + j - 1]);
        }
        startZ = (updatedPlayerChunkZ - 1) * CHUNK_LENGTH;
        chunkIndex = 0;
    }

    for (int i = 0; i < RENDER_DISTANCE; i++)
    {
        ChunkData chunkData = initChunkData((updatedPlayerChunkX + i) * CHUNK_LENGTH, startZ);
        chunks[i * RENDER_DISTANCE + chunkIndex] = std::make_unique<ChunkMesh>(chunkData);
        chunks[i * RENDER_DISTANCE + chunkIndex]->initMesh();
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
    return (chunks[x * RENDER_DISTANCE + y]);
}