#include "WorldUpdater.hpp"
#include "../../../globals.hpp"
#include "../../ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"
#include <chrono>
#include <iostream>
WorldUpdater::WorldUpdater()
{
    chunkLoadedData = std::make_unique<std::unique_ptr<ChunkData>[]>(RENDER_DISTANCE_2X * RENDER_DISTANCE_2X);
    for (int i = 0; i < RENDER_DISTANCE_2X; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
            chunkLoadedData[i * RENDER_DISTANCE_2X + j] = NULL;
    }
    stopThread = false;
    playerChunkCoord.x = 0;
    playerChunkCoord.y = 0;
    oldPlayerChunkCoord = playerChunkCoord;
    std::thread newUpdaterThread(&WorldUpdater::loadNewChunks, this);
    updaterThread.swap(newUpdaterThread);
}

WorldUpdater::~WorldUpdater()
{
    {
        std::lock_guard<std::mutex> stopThreadGuard(stopThreadMutex);
        stopThread = true;
    }
    updaterThread.join();
}

void WorldUpdater::loadNewChunks()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> stopThreadGuard(stopThreadMutex);
            if (stopThread)
                break;
        }

        std::vector<glm::vec2> ChunksToLoadCopy;
        std::vector<int> ChunksToReloadCopy;
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            ChunksToLoadCopy = chunksToLoad;
            ChunksToReloadCopy = chunksToReload;
            chunksToLoad.clear();
            chunksToReload.clear();
        }
        glm::vec2 playerChunkCoordCopy;
        {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            playerChunkCoordCopy = playerChunkCoord;
        }
        moveChunkLoaded(playerChunkCoordCopy);
        for (size_t i = 0; i < ChunksToLoadCopy.size(); i++)
        {
            int chunkCoordX = ChunksToLoadCopy[i].x;
            int chunkCoordZ = ChunksToLoadCopy[i].y;
            int arrayX = chunkCoordX / CHUNK_LENGTH - playerChunkCoordCopy.x + RENDER_DISTANCE;
            int arrayZ = chunkCoordZ / CHUNK_LENGTH - playerChunkCoordCopy.y + RENDER_DISTANCE;
            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
                continue;
            {
                std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
                chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] =
                    std::make_unique<ChunkData>(ChunkGenerator::GenerateChunkData(chunkCoordX, chunkCoordZ));
            }
            {

                ChunkMesh chunkMesh(*(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ].get()));
                chunkMesh.initMesh();
                std::lock_guard<std::mutex> chunksLoadedGuard(chunksLoadedMutex);
                chunksLoaded.push_back(chunkMesh);
            }
        }
    }
}

void WorldUpdater::moveChunkLoaded(const glm::vec2 &playerChunkCoordCopy)
{
    if (oldPlayerChunkCoord == playerChunkCoordCopy)
        return;

    std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
    if (oldPlayerChunkCoord.x != playerChunkCoordCopy.x)
        moveChunkAxisX(playerChunkCoordCopy);
    if (oldPlayerChunkCoord.y != playerChunkCoordCopy.y)
        moveChunkAxisZ(playerChunkCoordCopy);
    oldPlayerChunkCoord = playerChunkCoord;
}

void WorldUpdater::moveChunkAxisX(const glm::vec2 &playerChunkCoordCopy)
{
    int chunkToUpdateX;
    if (oldPlayerChunkCoord.x < playerChunkCoordCopy.x)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = RENDER_DISTANCE_2X - 1;
    }
    else if (oldPlayerChunkCoord.x > playerChunkCoordCopy.x)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = 0;
    }
    if (oldPlayerChunkCoord.x != playerChunkCoordCopy.x)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
            chunkLoadedData[chunkToUpdateX * RENDER_DISTANCE_2X + j] = NULL;
    }
}
void WorldUpdater::moveChunkAxisZ(const glm::vec2 &playerChunkCoordCopy)
{
    int chunkToUpdateZ;

    if (oldPlayerChunkCoord.y < playerChunkCoordCopy.y)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j + 1]);
        }
        chunkToUpdateZ = RENDER_DISTANCE_2X - 1;
    }
    else if (oldPlayerChunkCoord.y > playerChunkCoordCopy.y)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j - 1]);
        }
        chunkToUpdateZ = 0;
    }
    if (oldPlayerChunkCoord.y != playerChunkCoordCopy.y)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
            chunkLoadedData[i * RENDER_DISTANCE_2X + chunkToUpdateZ] = NULL;
    }
}

// @todo: find better names for argument
bool WorldUpdater::addChunkToLoad(const std::vector<glm::vec2> &chunksToLoadToAdd,
                                  const std::vector<int> &chunksToReloadToAdd)
{
    std::unique_lock<std::mutex> chunksToLoadGuard(chunksToLoadMutex, std::defer_lock);

    if (chunksToLoadGuard.try_lock())
    {
        for (size_t i = 0; i < chunksToLoadToAdd.size(); i++)
        {
            chunksToLoad.push_back(chunksToLoadToAdd[i]);
#ifdef DEBUG_MODE
            std::cout << "new chunk to load: " << chunksToLoadToAdd[i].first / CHUNK_LENGTH << " "
                      << chunksToLoadToAdd[i].second / CHUNK_LENGTH << std::endl;
#endif
        }
        for (size_t i = 0; i < chunksToReloadToAdd.size(); i++)
            chunksToReload.push_back(chunksToReloadToAdd[i]);
        return (true);
    }
    else
        return (false);
}

std::optional<std::vector<ChunkMesh>> WorldUpdater::getChunkLoaded()
{
    std::unique_lock<std::mutex> chunksLoadedGuard(chunksLoadedMutex, std::defer_lock);
    std::optional<std::vector<ChunkMesh>> chunks;
    if (chunksLoadedGuard.try_lock() && !chunksLoaded.empty())
    {
        chunks = chunksLoaded;
        chunksLoaded.clear();
    }
    return (chunks);
}

bool WorldUpdater::updatePlayerChunkCoord(const glm::vec2 &updatedPlayerChunkCoord)
{
    std::unique_lock<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex, std::defer_lock);
    if (playerChunkCoordGuard.try_lock())
    {
        playerChunkCoord = updatedPlayerChunkCoord;
        return (true);
    }
    else
        return (false);
}