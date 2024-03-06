#include "ChunkData.hpp"
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
                    this->blocks[x * CHUNK_LENGTH + y * CHUNK_HEIGHT + z] = instance.getBlock(x, y, z);
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
    return (blocks[x * CHUNK_LENGTH + y * CHUNK_HEIGHT + z]);
}

void ChunkData::addBlock(const BlockData &block)
{
    blocks[block.getX() * CHUNK_LENGTH + block.getY() * CHUNK_HEIGHT + block.getZ()] = block;
}
