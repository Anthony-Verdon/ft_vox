#include "ChunkData.hpp"
#include "../../../assert.hpp"
#include "../../../globals.hpp"
#include "../ChunkMesh/ChunkMesh.hpp"
#include <iostream>
#include <memory>

ChunkData::ChunkData(int chunkCoordX, int chunkCoordZ)
{
    *this = ChunkData(glm::vec2(chunkCoordX, chunkCoordZ));
}

ChunkData::ChunkData(const glm::vec2 &chunkCoord)
{
    this->chunkCoord = chunkCoord;
    blocks = std::make_unique<BlockType[]>(CHUNK_ARRAY_SIZE);
    for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
        blocks[i] = BlockType::AIR;
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
        blocks = std::make_unique<BlockType[]>(CHUNK_ARRAY_SIZE);
        for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
            this->blocks[i] = instance.getBlock(i);
    }
    return (*this);
}

ChunkData &ChunkData::operator=(const ChunkMesh &instance)
{
    chunkCoord = instance.getChunkCoord();
    blocks = std::make_unique<BlockType[]>(CHUNK_ARRAY_SIZE);
    for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
        this->blocks[i] = instance.getBlock(i);
    return (*this);
}

ChunkData::~ChunkData()
{
}

BlockType ChunkData::getBlock(int x, int y, int z) const
{
    return (getBlock(glm::vec3(x, y, z)));
}

BlockType ChunkData::getBlock(const glm::vec3 &coords) const
{
    return (getBlock(convert3DcoordsInto1Dcoords(coords)));
}

BlockType ChunkData::getBlock(unsigned int arrayCoord) const
{
    ASSERT_RETURN_VALUE(arrayCoord >= CHUNK_ARRAY_SIZE, "ChunkData::getBlock : coords given are out of bound",
                        BlockType::AIR);
    return (blocks[arrayCoord]);
}

void ChunkData::setBlock(int x, int y, int z, BlockType type)
{
    setBlock(glm::vec3(x, y, z), type);
}
void ChunkData::setBlock(const glm::vec3 &coords, BlockType type)
{
    const glm::vec3 blockCoordsInChunk = glm::vec3(convertWorldCoordIntoChunkCoords(coords.x, chunkCoord.x), coords.y,
                                                   convertWorldCoordIntoChunkCoords(coords.z, chunkCoord.y));
    const unsigned int arrayCoord = convert3DcoordsInto1Dcoords(blockCoordsInChunk);
    ASSERT_RETURN_VOID(arrayCoord >= CHUNK_ARRAY_SIZE, "ChunkData::setBlock : coords given are out of bound");
    ASSERT_RETURN_VOID(type != BlockType::AIR && blocks[arrayCoord] != BlockType::AIR,
                       "ChunkData::setBlock : Block already define at this position"
                           << std::endl
                           << "Block coords: " << coords.x << " " << coords.y << " " << coords.z << std::endl
                           << "Chunk coords: " << blockCoordsInChunk.x << " " << coords.y << " "
                           << blockCoordsInChunk.z)

    blocks[arrayCoord] = type;
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

unsigned int ChunkData::convert3DcoordsInto1Dcoords(int x, int y, int z)
{
    return (convert3DcoordsInto1Dcoords(glm::vec3(x, y, z)));
}

unsigned int ChunkData::convert3DcoordsInto1Dcoords(const glm::vec3 &coords)
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