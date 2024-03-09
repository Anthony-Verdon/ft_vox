#pragma once

#include "../ChunkClasses/ChunkMesh/ChunkMesh.hpp"

constexpr int RENDER_DISTANCE = 2;

class WorldData
{
  private:
    std::vector<std::unique_ptr<ChunkMesh>> chunks;

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateChunksLoad(int x, int y);
    const std::unique_ptr<ChunkMesh> &getChunk(int x, int y) const;
    size_t getNbChunks() const;
};