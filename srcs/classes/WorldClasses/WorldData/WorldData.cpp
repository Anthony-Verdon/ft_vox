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

    playerChunkCoord = {0, 0};
    updatePlayerCoord = true;
}

WorldData::~WorldData()
{
}

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
        updatePlayerCoord = worldUpdater.updatePlayerChunkCoord(playerChunkCoord);

    // get chunk loaded by WorldUpdater
    std::unique_ptr<ChunkMesh> chunkMesh = worldUpdater.getChunkLoaded();
    if (chunkMesh)
    {
        int arrayX = chunkMesh->getChunkCoordX() - playerChunkCoord.x + RENDER_DISTANCE;
        int arrayZ = chunkMesh->getChunkCoordZ() - playerChunkCoord.y + RENDER_DISTANCE;

        if (arrayX >= 0 && arrayX < RENDER_DISTANCE_2X && arrayZ >= 0 && arrayZ < RENDER_DISTANCE_2X)
            chunks[arrayX * RENDER_DISTANCE_2X + arrayZ] = std::make_unique<ChunkRenderer>(*chunkMesh);
    }
    // check if we need to load new chunks
    if (x < 0)
        x = x - CHUNK_LENGTH;
    if (z < 0)
        z = z - CHUNK_LENGTH;

    glm::vec2 updatedPlayerChunkCoord = {static_cast<int>(x) / CHUNK_LENGTH, static_cast<int>(z) / CHUNK_LENGTH};
    if (playerChunkCoord == updatedPlayerChunkCoord)
        return;

    if (playerChunkCoord.x != updatedPlayerChunkCoord.x)
        updateChunkAxisX(updatedPlayerChunkCoord);
    if (playerChunkCoord.y != updatedPlayerChunkCoord.y)
        updateChunkAxisZ(updatedPlayerChunkCoord);

    playerChunkCoord = updatedPlayerChunkCoord;
    updatePlayerCoord = true;
}

void WorldData::updateChunkAxisX(const glm::vec2 &updatedPlayerChunkCoord)
{
    int chunkArrayCoord;
    int startX;
    if (playerChunkCoord.x < updatedPlayerChunkCoord.x)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkCoord.x + RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkArrayCoord = RENDER_DISTANCE_2X - 1;
    }
    else if (playerChunkCoord.x > updatedPlayerChunkCoord.x)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        startX = (updatedPlayerChunkCoord.x - RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkArrayCoord = 0;
    }

    for (int j = 0; j < RENDER_DISTANCE_2X; j++)
    {
        chunks[chunkArrayCoord * RENDER_DISTANCE_2X + j] = NULL;
        chunksToLoad.push_back({startX, (updatedPlayerChunkCoord.y + j - RENDER_DISTANCE) * CHUNK_LENGTH});
    }
}

void WorldData::updateChunkAxisZ(const glm::vec2 &updatedPlayerChunkCoord)
{
    int chunkArrayCoord;
    int startZ;
    if (playerChunkCoord.y < updatedPlayerChunkCoord.y)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j + 1]);
        }
        startZ = (updatedPlayerChunkCoord.y + RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkArrayCoord = RENDER_DISTANCE_2X - 1;
    }
    else if (playerChunkCoord.y > updatedPlayerChunkCoord.y)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunks[i * RENDER_DISTANCE_2X + j] = std::move(chunks[i * RENDER_DISTANCE_2X + j - 1]);
        }
        startZ = (updatedPlayerChunkCoord.y - RENDER_DISTANCE) * CHUNK_LENGTH;
        chunkArrayCoord = 0;
    }

    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        chunks[i * RENDER_DISTANCE_2X + chunkArrayCoord] = NULL;
        chunksToLoad.push_back({(updatedPlayerChunkCoord.x + i - RENDER_DISTANCE) * CHUNK_LENGTH, startZ});
    }
}

const std::unique_ptr<ChunkRenderer> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE_2X + y]);
}