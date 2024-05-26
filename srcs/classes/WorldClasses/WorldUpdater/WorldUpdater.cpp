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
    playerChunkX = 0;
    playerChunkZ = 0;
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

        moveChunkLoaded();

        std::vector<std::pair<int, int>> ChunksToLoadCopy;
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            ChunksToLoadCopy = chunksToLoad;
            chunksToLoad.clear();
        }
        std::pair<int, int> playerChunkCoord;
        {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            playerChunkCoord.first = playerChunkX;
            playerChunkCoord.second = playerChunkZ;
        }
        generateChunkData(ChunksToLoadCopy, playerChunkCoord);
        generateChunkMesh(ChunksToLoadCopy, playerChunkCoord);
    }
}

void WorldUpdater::moveChunkLoaded()
{
    std::pair<int, int> playerChunkCoordCopy;
    {
        std::unique_lock<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
        playerChunkCoordCopy.first = playerChunkX;
        playerChunkCoordCopy.second = playerChunkZ;
    }

    if (oldPlayerChunkX == playerChunkCoordCopy.first && oldPlayerChunkZ == playerChunkCoordCopy.second)
        return;

    std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
    int chunkToUpdateX;
    int chunkToUpdateZ;
    if (oldPlayerChunkX < playerChunkCoordCopy.first)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = RENDER_DISTANCE_2X - 1;
    }
    else if (oldPlayerChunkX > playerChunkCoordCopy.first)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = 0;
    }
    if (oldPlayerChunkX != playerChunkCoordCopy.first)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
            chunkLoadedData[chunkToUpdateX * RENDER_DISTANCE_2X + j] = NULL;
        oldPlayerChunkX = playerChunkCoordCopy.first;
    }

    if (oldPlayerChunkZ < playerChunkCoordCopy.second)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j + 1]);
        }
        chunkToUpdateZ = RENDER_DISTANCE_2X - 1;
    }
    else if (oldPlayerChunkZ > playerChunkCoordCopy.second)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                    std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j - 1]);
        }
        chunkToUpdateZ = 0;
    }
    if (oldPlayerChunkZ != playerChunkCoordCopy.second)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
            chunkLoadedData[i * RENDER_DISTANCE_2X + chunkToUpdateZ] = NULL;
        oldPlayerChunkZ = playerChunkCoordCopy.second;
    }
}

void WorldUpdater::generateChunkData(const std::vector<std::pair<int, int>> &chunksToLoadCopy,
                                     const std::pair<int, int> &playerChunkCoord)
{
    for (size_t i = 0; i < chunksToLoadCopy.size(); i++)
    {
        int chunkCoordX = chunksToLoadCopy[i].first;
        int chunkCoordZ = chunksToLoadCopy[i].second;
        int arrayX = chunkCoordX / 16 - playerChunkCoord.first + RENDER_DISTANCE;
        int arrayZ = chunkCoordZ / 16 - playerChunkCoord.second + RENDER_DISTANCE;
        if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
            continue;
        std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
        chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] =
            std::make_unique<ChunkData>(ChunkGenerator::GenerateChunkData(chunkCoordX, chunkCoordZ));
    }
}

void WorldUpdater::generateChunkMesh(const std::vector<std::pair<int, int>> &chunksToLoadCopy,
                                     const std::pair<int, int> &playerChunkCoord)
{
    for (size_t i = 0; i < chunksToLoadCopy.size(); i++)
    {
        int arrayX = chunksToLoadCopy[i].first / 16 - playerChunkCoord.first + RENDER_DISTANCE;
        int arrayZ = chunksToLoadCopy[i].second / 16 - playerChunkCoord.second + RENDER_DISTANCE;
        if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X ||
            chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] == NULL)
            continue;

        std::array<std::optional<ChunkData>, 4> neighborsChunks;
        if (arrayX - 1 >= 0 && chunkLoadedData[(arrayX - 1) * RENDER_DISTANCE_2X + arrayZ] != NULL)
            neighborsChunks[0] = *(chunkLoadedData[(arrayX - 1) * RENDER_DISTANCE_2X + arrayZ].get());
        if (arrayX + 1 < RENDER_DISTANCE_2X && chunkLoadedData[(arrayX + 1) * RENDER_DISTANCE_2X + arrayZ] != NULL)
            neighborsChunks[1] = *(chunkLoadedData[(arrayX + 1) * RENDER_DISTANCE_2X + arrayZ].get());
        if (arrayZ - 1 >= 0 && chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ - 1] != NULL)
            neighborsChunks[2] = *(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ - 1].get());
        if (arrayZ + 1 < RENDER_DISTANCE_2X && chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ + 1] != NULL)
            neighborsChunks[3] = *(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ + 1].get());

        ChunkMesh chunkMesh(*(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ].get()));
        chunkMesh.initMesh(neighborsChunks);
        std::lock_guard<std::mutex> chunksLoadedGuard(chunksLoadedMutex);
        chunksLoaded.push_back(chunkMesh);
    }
}
bool WorldUpdater::addChunkToLoad(const std::vector<std::pair<int, int>> &chunksToLoadToAdd)
{
    std::unique_lock<std::mutex> chunksToLoadGuard(chunksToLoadMutex, std::defer_lock);

    if (chunksToLoadGuard.try_lock())
    {
        // std::cout << "new chunk to load: " << x / 16 << " " << z / 16 << std::endl;
        for (size_t i = 0; i < chunksToLoadToAdd.size(); i++)
            chunksToLoad.push_back(chunksToLoadToAdd[i]);
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

bool WorldUpdater::updatePlayerChunkCoord(int updatedPlayerChunkX, int updatedPlayerChunkZ)
{
    std::unique_lock<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex, std::defer_lock);
    if (playerChunkCoordGuard.try_lock())
    {
        playerChunkX = updatedPlayerChunkX;
        playerChunkZ = updatedPlayerChunkZ;
        return (true);
    }
    else
        return (false);
}