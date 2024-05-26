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
    // first = x, second = z
    std::pair<int, int> oldPlayerChunkCoord;
    std::pair<int, int> playerChunkCoord;

    std::mutex chunksToLoadMutex;
    std::vector<std::pair<int, int>> chunksToLoad;
    std::vector<int> chunksToReload;

    std::mutex chunksLoadedMutex;
    std::vector<ChunkMesh> chunksLoaded;

    std::mutex stopThreadMutex;
    bool stopThread;

    void loadNewChunks();
    void moveChunkLoaded(const std::pair<int, int> &playerChunkCoordCopy);
    void moveChunkAxisX(const std::pair<int, int> &playerChunkCoordCopy);
    void moveChunkAxisZ(const std::pair<int, int> &playerChunkCoordCopy);
    void generateChunkData(const std::vector<std::pair<int, int>> &chunksToLoadCopy,
                           const std::pair<int, int> &playerChunkCoordCopy);
    void generateChunkMesh(const std::pair<int, int> &arrayCoord);

  public:
    WorldUpdater();
    ~WorldUpdater();
    bool addChunkToLoad(const std::vector<std::pair<int, int>> &chunksToLoadToAdd,
                        const std::vector<int> &chunksToReloadToAdd);
    std::optional<std::vector<ChunkMesh>> getChunkLoaded();
    bool updatePlayerChunkCoord(const std::pair<int, int> &updatedPlayerChunkCoord);
};