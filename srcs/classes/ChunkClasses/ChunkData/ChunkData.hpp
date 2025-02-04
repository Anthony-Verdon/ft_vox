#pragma once

class ChunkMesh;

#include "../../BlockClasses/BlockData/BlockData.hpp"
#include "../../BlockClasses/BlockDico/BlockDico.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <optional>
class ChunkData
{
  protected:
    glm::vec2 chunkCoord;

  public:
    std::unique_ptr<BlockType[]> blocks;
    ChunkData(int chunkCoordX, int chunkCoordZ);
    ChunkData(const glm::vec2 &chunkCoord);
    ChunkData(const ChunkData &instance);
    ChunkData &operator=(const ChunkData &instance);
    ChunkData &operator=(const ChunkMesh &instance);
    ~ChunkData();

    void setBlock(int x, int y, int z, BlockType type, bool worldCoord);
    void setBlock(const glm::vec3 &blockCoord, BlockType type, bool worldCoord);
    BlockType getBlock(int x, int y, int z, bool worldCoord) const;
    BlockType getBlock(const glm::vec3 &blockCoord, bool worldCoord) const;
    BlockType getBlock(unsigned int arrayCoord) const;
    glm::vec2 getChunkCoord() const;
    int getChunkCoordX() const;
    int getChunkCoordZ() const;

    static unsigned int convertWorldCoordIntoChunkCoords(int coord, int chunkCoord);
    static unsigned int convert3DcoordsInto1Dcoords(int x, int y, int z);
    static unsigned int convert3DcoordsInto1Dcoords(const glm::vec3 &coords);
};