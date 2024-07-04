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
    COUNT
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