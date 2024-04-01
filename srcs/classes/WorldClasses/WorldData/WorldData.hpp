#pragma once

#include "../../ChunkClasses/ChunkMesh/ChunkMesh.hpp"
#include "../WorldUpdater/WorldUpdater.hpp"

class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkMesh>[]> chunks;
    WorldUpdater worldUpdater;
    
  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateChunksLoad(float x, float y);
    void updateChunkAxisX(int playerChunkX, int updatedPlayerChunkX, int updatedPlayerChunkZ);
    void updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ);
    const std::unique_ptr<ChunkMesh> &getChunk(int x, int y) const;
};