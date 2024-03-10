#pragma once

#include "../ChunkClasses/ChunkMesh/ChunkMesh.hpp"

constexpr int RENDER_DISTANCE = 5;

class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkMesh>[]> chunks;
    std::unique_ptr<ChunkData[]> chunksData;

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateChunksLoad(float x, float y);
    const std::unique_ptr<ChunkMesh> &getChunk(int x, int y) const;
};