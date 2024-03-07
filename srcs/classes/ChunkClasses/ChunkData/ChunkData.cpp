#include "ChunkData.hpp"
#include <cstdlib>
#include <memory>

ChunkData::ChunkData()
{
    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH);
}

ChunkData::ChunkData(const ChunkData &instance)
{
    *this = instance;
}

ChunkData &ChunkData::operator=(const ChunkData &instance)
{
    if (this != &instance)
    {
        blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH);
        for (int x = 0; x < CHUNK_LENGTH; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_LENGTH; z++)
                {
                    this->blocks[std::abs(x) % CHUNK_LENGTH * CHUNK_LENGTH + std::abs(y) % CHUNK_HEIGHT * CHUNK_HEIGHT +
                                 std::abs(z) % CHUNK_LENGTH] = instance.getBlock(x, y, z);
                }
            }
        }
    }
    return (*this);
}

ChunkData::~ChunkData()
{
}

std::optional<BlockData> ChunkData::getBlock(int x, int y, int z) const
{
    return (blocks[x % CHUNK_LENGTH * CHUNK_LENGTH + y % CHUNK_HEIGHT * CHUNK_HEIGHT + z % CHUNK_LENGTH]);
}

void ChunkData::addBlock(const BlockData &block)
{
    blocks[std::abs(block.getX()) % CHUNK_LENGTH * CHUNK_LENGTH + std::abs(block.getY()) % CHUNK_HEIGHT * CHUNK_HEIGHT +
           std::abs(block.getZ()) % CHUNK_LENGTH] = block;
}
