#include "BlockDico.hpp"

std::vector<BlockData> BlockDico::blocks;

void BlockDico::initBlockDico()
{
    blocks.reserve(BlockType::BLOCK_TYPE_COUNT);

    // grass
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternGrass;
    for (int i = 0; i < 6; i++)
        texturePatternGrass[i] = {0, 2};  // side
    texturePatternGrass[BOTTOM] = {1, 2}; // bottom
    texturePatternGrass[TOP] = {0, 1};    // top
    blocks[BlockType::GRASS] = BlockData(0, 0, 0, texturePatternGrass, false);

    // dirt
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternDirt;
    for (int i = 0; i < 6; i++)
        texturePatternDirt[i] = {1, 2};
    blocks[BlockType::DIRT] = BlockData(0, 0, 0, texturePatternDirt, false);

    // stone
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternStone;
    for (int i = 0; i < 6; i++)
        texturePatternStone[i] = {2, 2};
    blocks[BlockType::STONE] = BlockData(0, 0, 0, texturePatternStone, false);

    // water
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternWater;
    for (int i = 0; i < 6; i++)
        texturePatternWater[i] = {1, 1};
    blocks[BlockType::WATER] = BlockData(0, 0, 0, texturePatternWater, true);

    // wood
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternWood;
    for (int i = 0; i < 6; i++)
        texturePatternWood[i] = {1, 0};  // side
    texturePatternWood[BOTTOM] = {0, 0}; // bottom
    texturePatternWood[TOP] = {0, 0};    // top
    blocks[BlockType::WOOD] = BlockData(0, 0, 0, texturePatternWood, false);

    // leaves
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePatternLeaves;
    for (int i = 0; i < 6; i++)
        texturePatternLeaves[i] = {2, 1}; // side
    blocks[BlockType::LEAVES] = BlockData(0, 0, 0, texturePatternLeaves, false);
}

BlockData BlockDico::getBlockData(BlockType type)
{
    return (blocks[type]);
}