#pragma once

#include "../BlockData/BlockData.hpp"
#include <vector>

enum BlockType
{
    AIR = -1,
    GRASS,
    DIRT,
    STONE,
    WATER,
    WOOD,
    LEAVES,
    BLOCK_TYPE_COUNT
};

class BlockDico
{
  private:
    static std::vector<BlockData> blocks;

  public:
    BlockDico() = delete;
    ~BlockDico() = delete;

    static void initBlockDico();
    static BlockData getBlockData(BlockType type);
};