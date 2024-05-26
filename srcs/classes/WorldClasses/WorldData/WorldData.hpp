#pragma once

#include "../../../globals.hpp"
#include "../../ChunkClasses/ChunkRenderer/ChunkRenderer.hpp"
#include "../WorldUpdater/WorldUpdater.hpp"

class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkRenderer>[]> chunks;
    int playerChunkX;
    int playerChunkZ;

    WorldUpdater worldUpdater;
    std::vector<std::pair<int, int>> chunksToLoad;
    bool updatePlayerCoord;

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateChunksLoad(float x, float y);
    void updateChunkAxisX(int playerChunkX, int updatedPlayerChunkX, int updatedPlayerChunkZ);
    void updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ);
    const std::unique_ptr<ChunkRenderer> &getChunk(int x, int y) const;

#ifdef DEBUG_MODE
    void DebugWriteMap() const;
#endif
};