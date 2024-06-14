#pragma once

class ChunkMesh;

#include "../../BlockClasses/BlockData/BlockData.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <optional>
class ChunkData
{
  protected:
    int coordX;
    int coordZ;

  public:
    std::unique_ptr<std::optional<BlockData>[]> blocks;
    ChunkData(int coordX, int coordZ);
    ChunkData(const ChunkData &instance);
    ChunkData &operator=(const ChunkData &instance);
    ChunkData &operator=(const ChunkMesh &instance);
    ~ChunkData();

    std::optional<BlockData> getBlock(int x, int y, int z) const;
    void addBlock(const BlockData &block);
    int getX() const;
    int getZ() const;

    static int convertCoordIntoChunkCoords(int coord, int chunkCoord);
};