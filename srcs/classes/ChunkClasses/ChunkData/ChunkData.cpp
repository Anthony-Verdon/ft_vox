#include "ChunkData.hpp"
#include "../../../globals.hpp"
#include "../ChunkMesh/ChunkMesh.hpp"
#include <iostream>
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
        coordX = instance.getX();
        coordZ = instance.getZ();
        blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH);
        for (int x = 0; x < CHUNK_LENGTH; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_LENGTH; z++)
                {
                    this->blocks[convertCoordIntoChunkCoords(x) * CHUNK_LENGTH + y * CHUNK_HEIGHT +
                                 convertCoordIntoChunkCoords(z)] = instance.getBlock(x, y, z);
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

    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH);
    for (int x = 0; x < CHUNK_LENGTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_LENGTH; z++)
            {
                this->blocks[convertCoordIntoChunkCoords(x) * CHUNK_LENGTH + y * CHUNK_HEIGHT +
                             convertCoordIntoChunkCoords(z)] = instance.getBlock(x, y, z);
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
    x = convertCoordIntoChunkCoords(x);
    z = convertCoordIntoChunkCoords(z);
    return (blocks[x * CHUNK_LENGTH + y * CHUNK_HEIGHT + z]);
}

void ChunkData::addBlock(const BlockData &block)
{
    int x = convertCoordIntoChunkCoords(block.getX());
    int z = convertCoordIntoChunkCoords(block.getZ());
    blocks[x * CHUNK_LENGTH + block.getY() * CHUNK_HEIGHT + z] = block;
    coordX = block.getX() / CHUNK_LENGTH;
    if (block.getX() < 0)
        coordX = coordX - 1;
    coordZ = block.getZ() / CHUNK_LENGTH;
    if (block.getZ() < 0)
        coordZ = coordZ - 1;
}

int ChunkData::convertCoordIntoChunkCoords(int coord) const
{
    coord = coord % CHUNK_LENGTH;
    if (coord < 0)
        coord += CHUNK_LENGTH;
    return (coord);
}
int ChunkData::getX() const
{
    return coordX;
};

int ChunkData::getZ() const
{
    return coordZ;
};