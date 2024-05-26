#pragma once

#include "../../ChunkClasses/ChunkMesh/ChunkMesh.hpp"
#include <mutex>
#include <stack>
#include <thread>

class WorldUpdater
{
  private:
    std::thread updaterThread;

    std::mutex chunksLoadedDataMutex;
    std::unique_ptr<std::unique_ptr<ChunkData>[]> chunkLoadedData;

    std::mutex playerChunkCoordMutex;
    int oldPlayerChunkX;
    int oldPlayerChunkZ;
    int playerChunkX;
    int playerChunkZ;

    std::mutex chunksToLoadMutex;
    std::vector<std::pair<int, int>> chunksToLoad;

    std::mutex chunksLoadedMutex;
    std::vector<ChunkMesh> chunksLoaded;

    std::mutex stopThreadMutex;
    bool stopThread;

    void loadNewChunks();
    void moveChunkLoaded();
    void generateChunkData(const std::vector<std::pair<int, int>> &chunksToLoadCopy,
                           const std::pair<int, int> &playerChunkCoord);
    void generateChunkMesh(const std::vector<std::pair<int, int>> &chunksToLoadCopy,
                           const std::pair<int, int> &playerChunkCoord);

  public:
    WorldUpdater();
    ~WorldUpdater();
    bool addChunkToLoad(const std::vector<std::pair<int, int>> &chunksToLoadToAdd);
    std::optional<std::vector<ChunkMesh>> getChunkLoaded();
    bool updatePlayerChunkCoord(int newPlayerChunkX, int newPlayerChunkZ);
};