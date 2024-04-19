#include "WorldUpdater.hpp"
#include "../../../globals.hpp"
#include <iostream>
#include <thread>
WorldUpdater::WorldUpdater()
{
    stopThread = false;
    std::thread updaterThread(&WorldUpdater::loadNewChunks, this);
    updaterThread.detach();
}

WorldUpdater::~WorldUpdater()
{
    std::lock_guard<std::mutex> stopThreadGuard(stopThreadMutex);
    stopThread = true;
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

        std::stack<std::array<int, 4>> newChunksToLoad;
        size_t nbChunks;
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            nbChunks = chunksToLoad.size();
            for (size_t i = 0; i < nbChunks; i++)
            {
                newChunksToLoad.push(chunksToLoad.top());
                chunksToLoad.pop();
            }
        }
        for (size_t i = 0; i < nbChunks; i++)
        {
            // time of sleep could be modified in the future
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> chunksLoadedGuard(chunksLoadedMutex);
            ChunkData chunkData = initChunkData(newChunksToLoad.top()[0], newChunksToLoad.top()[1]);
            chunkData.arrayX = newChunksToLoad.top()[2];
            chunkData.arrayZ = newChunksToLoad.top()[3];
            ChunkMesh chunkMesh(chunkData);

            /*
            todo: really init it
            I think the best way is to have an array one RENDER_DISTANCE_2X size + 1 * 2
            because another way would be to generate 4 chunks data next to chunk wanted,
            but even if it's in a thread, (so it's not visible in game),
            it would take more time to generate
            */
            std::array<std::optional<ChunkData>, 4> neighborsChunks;
            chunkMesh.initMesh(neighborsChunks);
            chunksLoaded.push(chunkMesh);
            newChunksToLoad.pop();
        }
    }
}

ChunkData WorldUpdater::initChunkData(int modifierX, int modifierZ)
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
        {
            for (int posZ = 0; posZ < CHUNK_LENGTH; posZ++)
            {
                // int height = perlinNoise[std::abs(modifierX + posX) * 1024 + std::abs(modifierZ + posZ)] *
                // CHUNK_HEIGHT;
                int height = 64;
                for (int posY = 0; posY < height; posY++)
                {
                    BlockData block(modifierX + posX, posY, modifierZ + posZ, texturePattern);
                    chunkData.addBlock(block);
                }
            }
        }
    }

    return (chunkData);
}

void WorldUpdater::addChunkToLoad(int x, int z, int arrayX, int arrayZ)
{
    std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
    // std::cout << "new chunk to load: " << x / 16 << " " << z / 16 << std::endl;
    chunksToLoad.push({x, z, arrayX, arrayZ});
}

// maybe we could just send all the chunk in one vector
std::optional<ChunkMesh> WorldUpdater::getChunkLoaded()
{
    std::unique_lock<std::mutex> chunksLoadedGuard(chunksLoadedMutex, std::defer_lock);
    std::optional<ChunkMesh> chunk;
    if (chunksLoadedGuard.try_lock() && chunksLoaded.size() != 0)
    {
        chunk = chunksLoaded.top();
        chunksLoaded.pop();
        // std::cout << "new chunk loaded: " << chunk->getX() << " " << chunk->getZ() << std::endl;
    }
    return (chunk);
}