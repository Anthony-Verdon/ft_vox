#pragma once

#include "../../ChunkClasses/ChunkMesh/ChunkMesh.hpp"
#include <mutex>
#include <stack>
#include <thread>

class WorldUpdater
{
  private:
    std::mutex chunksLoadedDataMutex;
    std::unique_ptr<std::unique_ptr<ChunkData>[]> chunkLoadedData;

    std::mutex playerChunkCoordMutex;
    int playerChunkX;
    int playerChunkZ;

    std::mutex chunksToLoadMutex;
    std::stack<std::pair<int, int>> chunksToLoad;

    std::mutex chunksLoadedMutex;
    std::stack<ChunkMesh> chunksLoaded;

    std::thread updaterThread;
    std::mutex stopThreadMutex;
    bool stopThread;

    void loadNewChunks();
    ChunkData initChunkData(int modifierX, int modifierZ);

  public:
    WorldUpdater();
    ~WorldUpdater();
    void addChunkToLoad(int chunkX, int chunkZ);
    std::optional<ChunkMesh> getChunkLoaded();
    void updatePlayerChunkCoord(int newPlayerChunkX, int newPlayerChunkZ);
};