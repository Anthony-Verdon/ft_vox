#pragma once

#include "../ChunkClasses/ChunkMesh/ChunkMesh.hpp"

constexpr int RENDER_DISTANCE = 2;
constexpr int RENDER_DISTANCE_2X = 2 * RENDER_DISTANCE + 1;

class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkMesh>[]> chunks;

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateChunksLoad(float x, float y);
    void updateChunkAxisX(int playerChunkX, int updatedPlayerChunkX, int updatedPlayerChunkZ);
    void updateChunkAxisZ(int updatedPlayerChunkX, int playerChunkZ, int updatedPlayerChunkZ);
    ChunkData initChunkData(int modifierX, int modifierZ);
    const std::unique_ptr<ChunkMesh> &getChunk(int x, int y) const;

    std::unique_ptr<float[]> perlinNoise;
};