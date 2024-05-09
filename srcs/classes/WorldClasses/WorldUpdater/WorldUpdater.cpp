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

        std::vector<std::pair<int, int>> newChunksToLoad;
        size_t nbChunks;
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            nbChunks = chunksToLoad.size();
            newChunksToLoad = chunksToLoad;
            chunksToLoad.clear();
        }
        int actualPlayerChunkX;
        int actualPlayerChunkZ;
        {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            actualPlayerChunkX = playerChunkX;
            actualPlayerChunkZ = playerChunkZ;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
        for (size_t i = 0; i < nbChunks; i++)
        {
            int chunkCoordX = newChunksToLoad[i].first;
            int chunkCoordZ = newChunksToLoad[i].second;
            int arrayX = chunkCoordX / 16 - actualPlayerChunkX + RENDER_DISTANCE;
            ;
            int arrayZ = chunkCoordZ / 16 - actualPlayerChunkZ + RENDER_DISTANCE;
            ;
            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
                continue;
            chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] =
                std::make_unique<ChunkData>(ChunkGenerator::GenerateChunkData(chunkCoordX, chunkCoordZ));
        }
        for (size_t i = 0; i < nbChunks; i++)
        {
            int arrayX = newChunksToLoad[i].first / 16 - actualPlayerChunkX + RENDER_DISTANCE;
            ;
            int arrayZ = newChunksToLoad[i].second / 16 - actualPlayerChunkZ + RENDER_DISTANCE;
            ;
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
}

void WorldUpdater::addChunkToLoad(int chunkX, int chunkZ)
{
    std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
    // std::cout << "new chunk to load: " << x / 16 << " " << z / 16 << std::endl;
    chunksToLoad.push_back({chunkX, chunkZ});
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

void WorldUpdater::updatePlayerChunkCoord(int updatedPlayerChunkX, int updatedPlayerChunkZ)
{
    int oldPlayerChunkX;
    int oldPlayerChunkZ;
    {
        std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
        oldPlayerChunkX = playerChunkX;
        oldPlayerChunkZ = playerChunkZ;
        playerChunkX = updatedPlayerChunkX;
        playerChunkZ = updatedPlayerChunkZ;
    }
    {
        std::lock_guard<std::mutex> chunkLoadedDataGuard(chunksLoadedDataMutex);
        int chunkToUpdateX;
        int chunkToUpdateZ;
        if (oldPlayerChunkX < updatedPlayerChunkX)
        {
            for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
            {
                for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                    chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                        std::move(chunkLoadedData[(i + 1) * RENDER_DISTANCE_2X + j]);
            }
            chunkToUpdateX = RENDER_DISTANCE_2X - 1;
        }
        else if (oldPlayerChunkX > updatedPlayerChunkX)
        {
            for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
            {
                for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                    chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                        std::move(chunkLoadedData[(i - 1) * RENDER_DISTANCE_2X + j]);
            }
            chunkToUpdateX = 0;
        }
        if (oldPlayerChunkX != updatedPlayerChunkX)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[chunkToUpdateX * RENDER_DISTANCE_2X + j] = NULL;
        }

        if (oldPlayerChunkZ < updatedPlayerChunkZ)
        {
            for (int i = 0; i < RENDER_DISTANCE_2X; i++)
            {
                for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                    chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                        std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j + 1]);
            }
            chunkToUpdateZ = RENDER_DISTANCE_2X - 1;
        }
        else if (oldPlayerChunkZ > updatedPlayerChunkZ)
        {
            for (int i = 0; i < RENDER_DISTANCE_2X; i++)
            {
                for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                    chunkLoadedData[i * RENDER_DISTANCE_2X + j] =
                        std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j - 1]);
            }
            chunkToUpdateZ = 0;
        }
        if (oldPlayerChunkZ != updatedPlayerChunkZ)
        {
            for (int i = 0; i < RENDER_DISTANCE_2X; i++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + chunkToUpdateZ] = NULL;
        }
    }
}