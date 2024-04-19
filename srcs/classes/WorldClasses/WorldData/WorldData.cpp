#include "WorldData.hpp"
#include "../../../globals.hpp"
#include <array>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <memory>
#include <utility>

WorldData::WorldData()
{
    chunks = std::make_unique<std::unique_ptr<ChunkRenderer>[]>(RENDER_DISTANCE_2X * RENDER_DISTANCE_2X);

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            chunks[i * RENDER_DISTANCE_2X + j] = NULL;
            worldUpdater.addChunkToLoad((i - RENDER_DISTANCE) * CHUNK_LENGTH, (j - RENDER_DISTANCE) * CHUNK_LENGTH, i,
                                        j);
        }
}

WorldData::~WorldData()
{
}

void WorldData::updateChunksLoad(float x, float z)
{
    while (true)
    {
        std::optional<ChunkMesh> chunkMesh = worldUpdater.getChunkLoaded();
        if (!chunkMesh.has_value())
            break;
        chunks[chunkMesh->arrayX * RENDER_DISTANCE_2X + chunkMesh->arrayZ] =
            std::make_unique<ChunkRenderer>(chunkMesh.value());
    }
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

    if (playerChunkX != updatedPlayerChunkX && playerChunkZ != updatedPlayerChunkZ)
        ;
    else if (playerChunkX != updatedPlayerChunkX)
        updateChunkAxisX(playerChunkX, updatedPlayerChunkX, updatedPlayerChunkZ);
    else if (playerChunkZ != updatedPlayerChunkZ)
        updateChunkAxisZ(updatedPlayerChunkX, playerChunkZ, updatedPlayerChunkZ);

    playerChunkX = updatedPlayerChunkX;
    playerChunkZ = updatedPlayerChunkZ;
}

void WorldData::display()
{
    std::cout << std::endl;
    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
        {
            if (chunks[i * RENDER_DISTANCE_2X + j] != NULL)
                std::cout << chunks[i * RENDER_DISTANCE_2X + j]->getX() << " "
                          << chunks[i * RENDER_DISTANCE_2X + j]->getZ();
            else
                std::cout << "not load";
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
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
        chunks[chunkToUpdate[0] * RENDER_DISTANCE_2X + j] = NULL;
        worldUpdater.addChunkToLoad(startX, (updatedPlayerChunkZ + j - RENDER_DISTANCE) * CHUNK_LENGTH,
                                    chunkToUpdate[0], j);
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
        chunks[i * RENDER_DISTANCE_2X + chunkToUpdate[0]] = NULL;
        worldUpdater.addChunkToLoad((updatedPlayerChunkX + i - RENDER_DISTANCE) * CHUNK_LENGTH, startZ, i,
                                    chunkToUpdate[0]);
    }
}

const std::unique_ptr<ChunkRenderer> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}