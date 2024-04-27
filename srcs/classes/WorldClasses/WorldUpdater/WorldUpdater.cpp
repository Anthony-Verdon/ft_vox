#include "WorldUpdater.hpp"
#include "../../../globals.hpp"
#include <iostream>
#include <thread>
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

        std::stack<std::pair<int, int>> newChunksToLoad;
        std::stack<std::pair<int, int>> newChunksToLoad2;
        size_t nbChunks;
        {
            std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
            nbChunks = chunksToLoad.size();
            for (size_t i = 0; i < nbChunks; i++)
            {
                newChunksToLoad.push(chunksToLoad.top());
                newChunksToLoad2.push(chunksToLoad.top());
                chunksToLoad.pop();
            }
        }
        for (size_t i = 0; i < nbChunks; i++)
        {
            int arrayX, arrayZ;
            {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            arrayX = newChunksToLoad2.top().first / 16 - playerChunkX + RENDER_DISTANCE;;
            arrayZ = newChunksToLoad2.top().second / 16 - playerChunkZ + RENDER_DISTANCE;;
            }
            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X)
            {
                newChunksToLoad2.pop();
                continue;
            }
            chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] = std::make_unique<ChunkData>(initChunkData(newChunksToLoad2.top().first, newChunksToLoad2.top().second));
            newChunksToLoad2.pop();
        }
        for (size_t i = 0; i < nbChunks; i++)
        {
            // time of sleep could be modified in the future
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            int arrayX, arrayZ;
            {
            std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);
            arrayX = newChunksToLoad.top().first / 16 - playerChunkX + RENDER_DISTANCE;;
            arrayZ = newChunksToLoad.top().second / 16 - playerChunkZ + RENDER_DISTANCE;;
            }
            if (arrayX < 0 || arrayX >= RENDER_DISTANCE_2X || arrayZ < 0 || arrayZ >= RENDER_DISTANCE_2X || chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ] == NULL)
            {
                newChunksToLoad.pop();
                continue;
            }
            ChunkMesh chunkMesh(*(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ].get()));

            std::array<std::optional<ChunkData>, 4> neighborsChunks;
            /*
            if (arrayX - 1 >= 0 && chunkLoadedData[(arrayX  - 1)* RENDER_DISTANCE_2X + arrayZ] != NULL)
                neighborsChunks[0] = *(chunkLoadedData[(arrayX  - 1)* RENDER_DISTANCE_2X + arrayZ].get());
            if (arrayX + 1 < RENDER_DISTANCE_2X && chunkLoadedData[(arrayX  + 1)* RENDER_DISTANCE_2X + arrayZ] != NULL)
                neighborsChunks[1] = *(chunkLoadedData[(arrayX + 1)* RENDER_DISTANCE_2X + arrayZ].get());
            if (arrayZ - 1 >= 0 && chunkLoadedData[arrayX* RENDER_DISTANCE_2X + arrayZ - 1] != NULL)
                neighborsChunks[2] = *(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ - 1].get());
            if (arrayZ + 1  < RENDER_DISTANCE_2X && chunkLoadedData[arrayX* RENDER_DISTANCE_2X + arrayZ + 1] != NULL)
                neighborsChunks[3] = *(chunkLoadedData[arrayX * RENDER_DISTANCE_2X + arrayZ + 1].get());
            */
            chunkMesh.initMesh(neighborsChunks);
            std::lock_guard<std::mutex> chunksLoadedGuard(chunksLoadedMutex);
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

void WorldUpdater::addChunkToLoad(int chunkX, int chunkZ)
{
    std::lock_guard<std::mutex> chunksToLoadGuard(chunksToLoadMutex);
    // std::cout << "new chunk to load: " << x / 16 << " " << z / 16 << std::endl;
    chunksToLoad.push({chunkX, chunkZ});
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

void WorldUpdater::updatePlayerChunkCoord(int updatedPlayerChunkX, int updatedPlayerChunkZ)
{
    std::lock_guard<std::mutex> playerChunkCoordGuard(playerChunkCoordMutex);

    int chunkToUpdateX;
    int chunkToUpdateZ;
    if (playerChunkX < updatedPlayerChunkX)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X - 1; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] = std::move(chunkLoadedData[(i + 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = RENDER_DISTANCE_2X - 1;
    }
    else if (playerChunkX > updatedPlayerChunkX)
    {
        for (int i = RENDER_DISTANCE_2X - 1; i >= 1; i--)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] = std::move(chunkLoadedData[(i - 1) * RENDER_DISTANCE_2X + j]);
        }
        chunkToUpdateX = 0;
    }
    if (playerChunkX != updatedPlayerChunkX)
    {
        for (int j = 0; j < RENDER_DISTANCE_2X; j++)
            chunkLoadedData[chunkToUpdateX * RENDER_DISTANCE_2X + j] = NULL;
    }

    if (playerChunkZ < updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = 0; j < RENDER_DISTANCE_2X - 1; j++)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] = std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j + 1]);
        }
        chunkToUpdateZ = RENDER_DISTANCE_2X - 1;
    }
    else if (playerChunkZ > updatedPlayerChunkZ)
    {
        for (int i = 0; i < RENDER_DISTANCE_2X; i++)
        {
            for (int j = RENDER_DISTANCE_2X - 1; j >= 1; j--)
                chunkLoadedData[i * RENDER_DISTANCE_2X + j] = std::move(chunkLoadedData[i * RENDER_DISTANCE_2X + j - 1]);
        }
        chunkToUpdateZ = 0;
    }
    if (playerChunkZ != updatedPlayerChunkZ)
    {
         for (int i = 0; i < RENDER_DISTANCE_2X; i++)
            chunkLoadedData[i * RENDER_DISTANCE_2X + chunkToUpdateZ] = NULL;
    }
    playerChunkX = updatedPlayerChunkX;
    playerChunkZ = updatedPlayerChunkZ;
}