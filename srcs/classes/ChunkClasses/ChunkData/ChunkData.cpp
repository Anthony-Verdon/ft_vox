#include "ChunkData.hpp"
#include "../../../assert.hpp"
#include "../../../globals.hpp"
#include "../ChunkMesh/ChunkMesh.hpp"
#include <iostream>
#include <memory>

ChunkData::ChunkData(glm::vec2 chunkCoord)
{
    this->chunkCoord = chunkCoord;
    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_ARRAY_SIZE);
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
        blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_ARRAY_SIZE);
        for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
            this->blocks[i] = instance.getBlock(i);
    }
    return (*this);
}

ChunkData &ChunkData::operator=(const ChunkMesh &instance)
{
    chunkCoord = instance.getChunkCoord();
    blocks = std::make_unique<std::optional<BlockData>[]>(CHUNK_ARRAY_SIZE);
    for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
        this->blocks[i] = instance.getBlock(i);
    return (*this);
}

ChunkData::~ChunkData()
{
}

std::optional<BlockData> ChunkData::getBlock(glm::vec3 coords) const
{
    return (getBlock(convert3DcoordsInto1Dcoords(coords)));
}

//@todo check if the return assert return a optional not set
std::optional<BlockData> ChunkData::getBlock(unsigned int arrayCoord) const
{
    std::optional<BlockData> notSet;
    ASSERT_RETURN_VALUE(arrayCoord >= CHUNK_ARRAY_SIZE, "ChunkData::getBlock : coords given are out of bound", notSet);
    return (blocks[arrayCoord]);
}

void ChunkData::addBlock(const BlockData &block)
{
    const glm::vec3 blockCoordsInChunk =
        glm::vec3(convertWorldCoordIntoChunkCoords(block.getX(), chunkCoord.x), block.getY(),
                  convertWorldCoordIntoChunkCoords(block.getZ(), chunkCoord.y));
    const unsigned int arrayCoord = convert3DcoordsInto1Dcoords(blockCoordsInChunk);

    ASSERT_RETURN_VOID(arrayCoord >= CHUNK_ARRAY_SIZE, "ChunkData::addBlock : coords given are out of bound");
    ASSERT_RETURN_VOID(blocks[arrayCoord].has_value(), "ChunkData::addBlock : Block already define at this position"
                                                           << std::endl
                                                           << "Block coords: " << block.getX() << " " << block.getY()
                                                           << " " << block.getZ() << std::endl
                                                           << "Chunk coords: " << blockCoordsInChunk.x << " "
                                                           << block.getY() << " " << blockCoordsInChunk.z)

    blocks[arrayCoord] = block;
}

unsigned int ChunkData::convertWorldCoordIntoChunkCoords(int coord, int chunkCoord)
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

unsigned int ChunkData::convert3DcoordsInto1Dcoords(glm::vec3 coords)
{
    return (coords.x + coords.y * CHUNK_LENGTH_PLUS_2 + coords.z * CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT);
}

glm::vec2 ChunkData::getChunkCoord() const
{
    return chunkCoord;
}

int ChunkData::getChunkCoordX() const
{
    return chunkCoord.x;
}

int ChunkData::getChunkCoordZ() const
{
    return chunkCoord.y;
}