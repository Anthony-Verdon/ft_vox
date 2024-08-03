#include "WorldUpdater.hpp"
#include "../../../globals.hpp"
#include "../../ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"
#include <chrono>
#include <iostream>
WorldUpdater::WorldUpdater()
{
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
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            ChunksToLoadCopy = chunksToLoad;
            chunksToLoad.clear();
        }
        glm::vec2 playerChunkCoordCopy;
        {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            playerChunkCoordCopy = playerChunkCoord;
        }
        for (size_t i = 0; i < ChunksToLoadCopy.size(); i++)
        {
            {
                std::lock_guard<std::mutex> stopThreadGuard(stopThreadMutex);
                if (stopThread)
                    return;
            }

            int chunkCoordX = ChunksToLoadCopy[i].x;
            int chunkCoordZ = ChunksToLoadCopy[i].y;
            int arrayX = chunkCoordX / CHUNK_LENGTH - playerChunkCoordCopy.x + RENDER_DISTANCE;
            int arrayZ = chunkCoordZ / CHUNK_LENGTH - playerChunkCoordCopy.y + RENDER_DISTANCE;
            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
                continue;

            {
                std::unique_ptr<ChunkMesh> chunkMesh =
                    std::make_unique<ChunkMesh>(ChunkGenerator::GenerateChunkData(chunkCoordX, chunkCoordZ));
                chunkMesh->initMesh();
                std::lock_guard<std::mutex> chunksLoadedGuard(chunksLoadedMutex);
                chunksLoaded.push_back(std::move(chunkMesh));
            }
        }
    }
}

bool WorldUpdater::addChunkToLoad(const std::vector<glm::vec2> &newChunksToLoad)
{
    std::unique_lock<std::mutex> chunksToLoadGuard(chunksToLoadMutex, std::defer_lock);

    if (chunksToLoadGuard.try_lock())
    {
        for (size_t i = 0; i < newChunksToLoad.size(); i++)
        {
            chunksToLoad.push_back(newChunksToLoad[i]);
#ifdef DEBUG_MODE
            std::cout << "new chunk to load: " << newChunksToLoad[i].x / CHUNK_LENGTH << " "
                      << newChunksToLoad[i].y / CHUNK_LENGTH << std::endl;
#endif
        }
        return (true);
    }
    else
        return (false);
}

std::unique_ptr<ChunkMesh> WorldUpdater::getChunkLoaded()
{
    std::unique_lock<std::mutex> chunksLoadedGuard(chunksLoadedMutex, std::defer_lock);
    std::unique_ptr<ChunkMesh> chunk = NULL;
    if (chunksLoadedGuard.try_lock() && !chunksLoaded.empty())
    {
        chunk = std::move(chunksLoaded[chunksLoaded.size() - 1]);
        chunksLoaded.pop_back();
    }
    return (chunk);
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