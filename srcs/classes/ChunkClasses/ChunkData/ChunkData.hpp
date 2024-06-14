#pragma once

class ChunkMesh;

#include "../../BlockClasses/BlockData/BlockData.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <optional>
class ChunkData
{
  protected:
    glm::vec2 chunkCoord;

  public:
    std::unique_ptr<std::optional<BlockData>[]> blocks;
    ChunkData(glm::vec2 chunkCoord);
    ChunkData(const ChunkData &instance);
    ChunkData &operator=(const ChunkData &instance);
    ChunkData &operator=(const ChunkMesh &instance);
    ~ChunkData();

    std::optional<BlockData> getBlock(glm::vec3 blockCoord) const;
    std::optional<BlockData> getBlock(unsigned int arrayCoord) const;
    void addBlock(const BlockData &block);
    glm::vec2 getChunkCoord() const;

    static unsigned int convertWorldCoordIntoChunkCoords(int coord, int chunkCoord);
    static unsigned int convert3DcoordsInto1Dcoords(glm::vec3 coords);
};