#include "ChunkData.hpp"
#include "../../../assert.hpp"
#include "../../../globals.hpp"
#include "../ChunkMesh/ChunkMesh.hpp"
#include <iostream>
#include <memory>

ChunkData::ChunkData(glm::vec2 chunkCoord)
{
    this->chunkCoord = chunkCoord;
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
        chunkCoord = instance.getChunkCoord();
        blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2);
        for (int x = 0; x < CHUNK_LENGTH_PLUS_2; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_LENGTH_PLUS_2; z++)
                {
                    this->blocks[x + y * CHUNK_LENGTH_PLUS_2 + z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT] =
                        instance.getBlock(glm::vec3(x, y, z));
                }
            }
        }
    }
    return (*this);
}

ChunkData &ChunkData::operator=(const ChunkMesh &instance)
{
    chunkCoord = instance.getChunkCoord();

    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2);
    for (int x = 0; x < CHUNK_LENGTH_PLUS_2; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_LENGTH_PLUS_2; z++)
            {
                this->blocks[x + y * CHUNK_LENGTH_PLUS_2 + z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT] =
                    instance.getBlock(glm::vec3(x, y, z));
            }
        }
    }
    return (*this);
}

ChunkData::~ChunkData()
{
}

std::optional<BlockData> ChunkData::getBlock(glm::vec3 coords) const
{
    return (blocks[convert3DcoordsInto1Dcoords(coords)]);
}

void ChunkData::addBlock(const BlockData &block)
{
    ASSERT_RETURN_VOID(blocks[convert3DcoordsInto1Dcoords(
                                  glm::vec3(convertCoordIntoChunkCoords(block.getX(), chunkCoord.x), block.getY(),
                                            convertCoordIntoChunkCoords(block.getZ(), chunkCoord.y)))]
                           .has_value(),
                       "ChunkData::addBlock : Block already define at this position"
                           << std::endl
                           << "Block coords: " << block.getX() << " " << block.getY() << " " << block.getZ()
                           << std::endl
                           << "Chunk coords: " << convertCoordIntoChunkCoords(block.getX(), chunkCoord.x) << " "
                           << block.getY() << " " << convertCoordIntoChunkCoords(block.getZ(), chunkCoord.y))

    blocks[convert3DcoordsInto1Dcoords(glm::vec3(convertCoordIntoChunkCoords(block.getX(), chunkCoord.x), block.getY(),
                                                 convertCoordIntoChunkCoords(block.getZ(), chunkCoord.y)))] = block;
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

int ChunkData::convert3DcoordsInto1Dcoords(glm::vec3 coords)
{
    return (coords.x + coords.y * CHUNK_LENGTH_PLUS_2 + coords.z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT);
}

glm::vec2 ChunkData::getChunkCoord() const
{
    return chunkCoord;
}
