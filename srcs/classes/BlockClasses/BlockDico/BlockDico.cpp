#include "BlockDico.hpp"

std::vector<BlockData> BlockDico::blocks;

void BlockDico::initBlockDico()
{
    blocks.reserve(BlockType::COUNT);

    // grass
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternGrass;
    for (int i = 0; i < 6; i++)
        texturePatternGrass[i] = {0, 2};  // side
    texturePatternGrass[BOTTOM] = {1, 2}; // bottom
    texturePatternGrass[TOP] = {0, 1};    // top
    blocks[BlockType::GRASS] = BlockData(0, 0, 0, texturePatternGrass);

    // dirt
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternDirt;
    for (int i = 0; i < 6; i++)
        texturePatternDirt[i] = {1, 2};
    blocks[BlockType::DIRT] = BlockData(0, 0, 0, texturePatternDirt);

    // stone
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternStone;
    for (int i = 0; i < 6; i++)
        texturePatternStone[i] = {2, 2};
    blocks[BlockType::STONE] = BlockData(0, 0, 0, texturePatternStone);

    // water
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternWater;
    for (int i = 0; i < 6; i++)
        texturePatternWater[i] = {1, 1};
    blocks[BlockType::WATER] = BlockData(0, 0, 0, texturePatternWater);
}

BlockData BlockDico::getBlockData(BlockType type)
{
    return (blocks[type]);
}