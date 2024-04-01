#include "WorldData.hpp"
#include <array>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <memory>
#include <utility>
#include "../../../globals.hpp"

WorldData::WorldData()
{
    chunks = std::make_unique<std::unique_ptr<ChunkMesh>[]>(RENDER_DISTANCE_2X * RENDER_DISTANCE_2X);

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            chunks[i * RENDER_DISTANCE_2X + j] = NULL;
            worldUpdater.addChunkToLoad(i, j);
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
    while (true)
    {
        std::optional<ChunkData>  chunkData = worldUpdater.getChunkLoaded();
        if (!chunkData.has_value())
            break;
        std::unique_ptr<ChunkMesh> chunkMesh = std::make_unique<ChunkMesh>(chunkData.value());
        std::array<std::optional<ChunkData>, 4> neighborsChunks;
        chunkMesh->initMesh(neighborsChunks);
        int x = chunkMesh->getX() + RENDER_DISTANCE;
        int z = chunkMesh->getZ() + RENDER_DISTANCE;
        chunks[x * RENDER_DISTANCE_2X + z] = std::move(chunkMesh);
    }
    return;

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
    (void)updatedPlayerChunkX;
    (void)playerChunkX;
    (void)updatedPlayerChunkZ;
    /*
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
    */
}

void WorldData::updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ)
{
    (void)updatedPlayerChunkX;
    (void)playerChunkZ;
    (void)updatedPlayerChunkZ;
    /*
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
    */
}

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}