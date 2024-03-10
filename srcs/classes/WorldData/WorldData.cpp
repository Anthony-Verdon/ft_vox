#include "WorldData.hpp"
#include <iostream>
#include <memory>

WorldData::WorldData()
{
    chunks = std::make_unique<std::unique_ptr<ChunkMesh>[]>(RENDER_DISTANCE * RENDER_DISTANCE);
    chunksData = std::make_unique<ChunkData[]>(RENDER_DISTANCE * RENDER_DISTANCE);

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
            chunksData[i * RENDER_DISTANCE + j] = chunkData;
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

    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    if (playerChunkX < updatedPlayerChunkX)
    {
        for (int i = 0; i < RENDER_DISTANCE - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[(i + 1) * RENDER_DISTANCE + j]);
        }
        for (int j = 0; j < RENDER_DISTANCE; j++)
        {
            ChunkData chunkData;
            for (int posX = 0; posX < CHUNK_LENGTH; posX++)
            {
                for (int posY = 0; posY < 1; posY++)
                {
                    for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
                    {
                        BlockData block((updatedPlayerChunkX + RENDER_DISTANCE - 1) * CHUNK_LENGTH + posX, posY,
                                        (updatedPlayerChunkZ + j) * CHUNK_LENGTH + posZ, texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            chunks[(RENDER_DISTANCE - 1) * RENDER_DISTANCE + j] = std::make_unique<ChunkMesh>(chunkData);
            chunks[(RENDER_DISTANCE - 1) * RENDER_DISTANCE + j]->initMesh();
        }
    }
    else if (playerChunkX > updatedPlayerChunkX)
    {
        for (int i = RENDER_DISTANCE - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[(i - 1) * RENDER_DISTANCE + j]);
        }
        for (int j = 0; j < RENDER_DISTANCE; j++)
        {
            ChunkData chunkData;
            for (int posX = 0; posX < CHUNK_LENGTH; posX++)
            {
                for (int posY = 0; posY < 1; posY++)
                {
                    for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
                    {
                        BlockData block((updatedPlayerChunkX - 1) * CHUNK_LENGTH + posX, posY,
                                        (updatedPlayerChunkZ + j) * CHUNK_LENGTH + posZ, texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            chunks[j] = std::make_unique<ChunkMesh>(chunkData);
            chunks[j]->initMesh();
        }
    }

    if (playerChunkZ < updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE - 1; j++)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[i * RENDER_DISTANCE + j + 1]);
        }
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            ChunkData chunkData;
            for (int posX = 0; posX < CHUNK_LENGTH; posX++)
            {
                for (int posY = 0; posY < 1; posY++)
                {
                    for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
                    {
                        BlockData block((updatedPlayerChunkX + i) * CHUNK_LENGTH + posX, posY,
                                        (updatedPlayerChunkZ + RENDER_DISTANCE - 1) * CHUNK_LENGTH + posZ,
                                        texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            chunks[i * RENDER_DISTANCE + RENDER_DISTANCE - 1] = std::make_unique<ChunkMesh>(chunkData);
            chunks[i * RENDER_DISTANCE + RENDER_DISTANCE - 1]->initMesh();
        }
    }
    else if (playerChunkZ > updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            for (int j = RENDER_DISTANCE - 1; j >= 1; j--)
                chunks[i * RENDER_DISTANCE + j] = std::move(chunks[i * RENDER_DISTANCE + j - 1]);
        }
        for (int i = 0; i < RENDER_DISTANCE; i++)
        {
            ChunkData chunkData;
            for (int posX = 0; posX < CHUNK_LENGTH; posX++)
            {
                for (int posY = 0; posY < 1; posY++)
                {
                    for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
                    {
                        BlockData block((updatedPlayerChunkX + i) * CHUNK_LENGTH + posX, posY,
                                        (updatedPlayerChunkZ - 1) * CHUNK_LENGTH + posZ, texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            chunks[i * RENDER_DISTANCE] = std::make_unique<ChunkMesh>(chunkData);
            chunks[i * RENDER_DISTANCE]->initMesh();
        }
    }
    playerChunkX = updatedPlayerChunkX;
    playerChunkZ = updatedPlayerChunkZ;
}

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE + y]);
}