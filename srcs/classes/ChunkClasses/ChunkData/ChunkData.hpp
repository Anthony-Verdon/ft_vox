#pragma once

#include "../../BlockClasses/BlockData/BlockData.hpp"
#include <memory>
#include <optional>

constexpr int CHUNK_LENGTH = 16;
constexpr int CHUNK_HEIGHT = 256;

class ChunkData
{
  protected:
    int x;
    int z;
    std::unique_ptr<std::optional<BlockData>[]> blocks;

  public:
    ChunkData();
    ChunkData(const ChunkData &instance);
    ChunkData &operator=(const ChunkData &instance);
    ~ChunkData();

    std::optional<BlockData> getBlock(int x, int y, int z) const;
    void addBlock(const BlockData &block);
    int getX() const;
    int getZ() const;

    static int convertCoordIntoChunkCoords(int coord);
};