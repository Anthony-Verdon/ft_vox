#pragma once

#include "../../ChunkClasses/ChunkMesh/ChunkMesh.hpp"
#include <glm/glm.hpp>
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
    glm::vec2 oldPlayerChunkCoord;
    glm::vec2 playerChunkCoord;

    std::mutex chunksToLoadMutex;
    std::vector<glm::vec2> chunksToLoad;

    std::mutex chunksLoadedMutex;
    std::vector<std::unique_ptr<ChunkMesh>> chunksLoaded;

    std::mutex stopThreadMutex;
    bool stopThread;

    void loadNewChunks();
    void moveChunkLoaded(const glm::vec2 &playerChunkCoordCopy);
    void moveChunkAxisX(const glm::vec2 &playerChunkCoordCopy);
    void moveChunkAxisZ(const glm::vec2 &playerChunkCoordCopy);

  public:
    WorldUpdater();
    ~WorldUpdater();

    bool addChunkToLoad(const std::vector<glm::vec2> &newChunksToLoad);
    std::unique_ptr<ChunkMesh> getChunkLoaded();
    bool updatePlayerChunkCoord(const glm::vec2 &updatedPlayerChunkCoord);
};