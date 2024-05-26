#pragma once

#include "../../../globals.hpp"
#include "../../ChunkClasses/ChunkRenderer/ChunkRenderer.hpp"
#include "../WorldUpdater/WorldUpdater.hpp"

class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkRenderer>[]> chunks;
    // first = x, second = z
    // todo, maybe instead of std pair create just a struct holding it, so I don't use first and second so it's more
    // clear
    std::pair<int, int> playerChunkCoord;

    WorldUpdater worldUpdater;
    std::vector<std::pair<int, int>> chunksToLoad;
    std::vector<int> chunksToReload;
    bool updatePlayerCoord;

    void updateChunkAxisX(const std::pair<int, int> &updatedPlayerChunkCoord);
    void updateChunkAxisZ(const std::pair<int, int> &updatedPlayerChunkCoord);

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateWorldData(float x, float y);
    const std::unique_ptr<ChunkRenderer> &getChunk(int x, int y) const;
};