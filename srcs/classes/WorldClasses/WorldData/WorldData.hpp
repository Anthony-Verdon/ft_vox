#pragma once

#include "../../../globals.hpp"
#include "../../ChunkClasses/ChunkRenderer/ChunkRenderer.hpp"
#include "../WorldUpdater/WorldUpdater.hpp"
#include <glm/glm.hpp>
class WorldData
{
  private:
    std::unique_ptr<std::unique_ptr<ChunkRenderer>[]> chunks;
    // first = x, second = z
    // todo, maybe instead of std pair create just a struct holding it, so I don't use first and second so it's more
    // clear
    glm::vec2 playerChunkCoord;

    WorldUpdater worldUpdater;
    std::vector<glm::vec2> chunksToLoad;
    std::vector<int> chunksToReload;
    bool updatePlayerCoord;

    void updateChunkAxisX(const glm::vec2 &updatedPlayerChunkCoord);
    void updateChunkAxisZ(const glm::vec2 &updatedPlayerChunkCoord);

  public:
    WorldData();
    WorldData(const WorldData &instance);
    WorldData &operator=(const WorldData &instance);
    ~WorldData();

    void updateWorldData(float x, float y);
    const std::unique_ptr<ChunkRenderer> &getChunk(int x, int y) const;
    const std::unique_ptr<ChunkRenderer> &getChunk(const glm::vec2 &coords) const;
};