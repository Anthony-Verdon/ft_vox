#include "ChunkData.hpp"
#include "../../../assert.hpp"
#include "../../../globals.hpp"
#include "../ChunkMesh/ChunkMesh.hpp"
#include <iostream>
#include <memory>

ChunkData::ChunkData(int coordX, int coordZ)
{
    this->coordX = coordX;
    this->coordZ = coordZ;
    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2);
}

ChunkData::ChunkData(const ChunkData &instance)
{
    *this = instance;
}

ChunkData &ChunkData::operator=(const ChunkData &instance)
{
    if (this != &instance)
    {
        coordX = instance.getX();
        coordZ = instance.getZ();
        blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2);
        for (int x = 0; x < CHUNK_LENGTH_PLUS_2; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_LENGTH_PLUS_2; z++)
                {
                    this->blocks[x + y * CHUNK_LENGTH_PLUS_2 + z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT] =
                        instance.getBlock(x, y, z);
                }
            }
        }
    }
    return (*this);
}

ChunkData &ChunkData::operator=(const ChunkMesh &instance)
{
    coordX = instance.getX();
    coordZ = instance.getZ();

    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2);
    for (int x = 0; x < CHUNK_LENGTH_PLUS_2; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_LENGTH_PLUS_2; z++)
            {
                this->blocks[x + y * CHUNK_LENGTH_PLUS_2 + z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT] =
                    instance.getBlock(x, y, z);
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
    return (blocks[x + y * CHUNK_LENGTH_PLUS_2 + z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT]);
}

void ChunkData::addBlock(const BlockData &block)
{
    ASSERT_RETURN_VOID(blocks[convertCoordIntoChunkCoords(block.getX(), coordX) + block.getY() * CHUNK_LENGTH_PLUS_2 +
                              convertCoordIntoChunkCoords(block.getZ(), coordZ) * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT]
                           .has_value(),
                       "ChunkData::addBlock : Block already define at this position"
                           << std::endl
                           << "Block coords: " << block.getX() << " " << block.getY() << " " << block.getZ()
                           << std::endl
                           << "Chunk coords: " << convertCoordIntoChunkCoords(block.getX(), coordX) << " "
                           << block.getY() << " " << convertCoordIntoChunkCoords(block.getZ(), coordZ))

    blocks[convertCoordIntoChunkCoords(block.getX(), coordX) + block.getY() * CHUNK_LENGTH_PLUS_2 +
           convertCoordIntoChunkCoords(block.getZ(), coordZ) * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT] = block;
}

int ChunkData::convertCoordIntoChunkCoords(int coord, int chunkCoord)
{
    if (coord >= chunkCoord * CHUNK_LENGTH && coord < (chunkCoord + 1) * CHUNK_LENGTH)
    {
        coord = coord % CHUNK_LENGTH;
        if (coord < 0)
            coord += CHUNK_LENGTH;
        return (coord + 1);
    }
    else if (coord < chunkCoord * CHUNK_LENGTH)
    {
        return (0);
    }
    else
    {
        return (CHUNK_LENGTH_PLUS_2 - 1);
    }
}

int ChunkData::getX() const
{
    return coordX;
};

int ChunkData::getZ() const
{
    return coordZ;
};