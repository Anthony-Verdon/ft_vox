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
            chunksToLoad.push_back({(i - RENDER_DISTANCE) * CHUNK_LENGTH, (j - RENDER_DISTANCE) * CHUNK_LENGTH});
        }

    playerChunkX = 0;
    playerChunkZ = 0;
    updatePlayerCoord = true;
}

WorldData::~WorldData()
{
}

// todo: rename it in UpdateWorldData and separate in multiples functions
void WorldData::updateWorldData(float x, float z)
{
    // update WorldUpdater informations (chunk to load, player position)
    if (chunksToLoad.size() > 0)
    {
        bool chunkAdded = worldUpdater.addChunkToLoad(chunksToLoad);
        if (chunkAdded)
            chunksToLoad.clear();
    }
    if (updatePlayerCoord)
        updatePlayerCoord = worldUpdater.updatePlayerChunkCoord(playerChunkX, playerChunkZ);

    // get chunk loaded by WorldUpdater
    std::optional<std::vector<ChunkMesh>> chunkMeshOptional = worldUpdater.getChunkLoaded();
    if (chunkMeshOptional.has_value())
    {
        std::vector<ChunkMesh> chunkMesh = chunkMeshOptional.value();
        for (size_t i = 0; i < chunkMesh.size(); i++)
        {
            int arrayX = chunkMesh[i].getX() - playerChunkX + RENDER_DISTANCE;
            int arrayZ = chunkMesh[i].getZ() - playerChunkZ + RENDER_DISTANCE;

            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
                continue;

            chunks[arrayX * RENDER_DISTANCE_2X + arrayZ] = std::make_unique<ChunkRenderer>(chunkMesh[i]);
        }
    }

    // check if we need to load new chunks
    if (x < 0)
        x = x - 16;
    if (z < 0)
        z = z - 16;

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
    updatePlayerCoord = true;
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
        chunksToLoad.push_back({startX, (updatedPlayerChunkZ + j - RENDER_DISTANCE) * CHUNK_LENGTH});
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
        chunksToLoad.push_back({(updatedPlayerChunkX + i - RENDER_DISTANCE) * CHUNK_LENGTH, startZ});
    }
}

const std::unique_ptr<ChunkRenderer> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}