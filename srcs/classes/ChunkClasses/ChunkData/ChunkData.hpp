#pragma once

#include "../../BlockClasses/BlockData/BlockData.hpp"
#include <memory>
#include <optional>
#define CHUNK_LENGTH 16
#define CHUNK_HEIGHT 256

class ChunkData
{
  protected:
    std::unique_ptr<std::optional<BlockData>[]> blocks;

  public:
    ChunkData();
    ChunkData(const ChunkData &instance);
    ChunkData &operator=(const ChunkData &instance);
    ~ChunkData();

    std::optional<BlockData> getBlock(int x, int y, int z) const;
    void addBlock(const BlockData &block);
};