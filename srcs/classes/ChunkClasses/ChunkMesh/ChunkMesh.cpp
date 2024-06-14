#include "ChunkMesh.hpp"
#include "../../../globals.hpp"
#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
ChunkMesh::ChunkMesh(const ChunkData &data) : ChunkData(data)
{
}

ChunkMesh::ChunkMesh(const ChunkMesh &instance) : ChunkData(instance)
{
    *this = instance;
}

ChunkMesh &ChunkMesh::operator=(const ChunkMesh &instance)
{
    if (this != &instance)
    {
        vertices = instance.getVertices();
        faces = instance.getFaces();
        chunkCoord = instance.getChunkCoord();
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

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::initMesh()
{
    faces.clear();
    vertices.clear();
    const std::array<int, 2> modifiers = {-1, 1};
    for (int i = 0; i < CHUNK_LENGTH_PLUS_2 * CHUNK_HEIGHT * CHUNK_LENGTH_PLUS_2; i++)
    {
        std::optional<BlockData> blockData = blocks[i];
        if (!blockData.has_value())
            continue;
        const int x = convertCoordIntoChunkCoords(blockData->getX(), chunkCoord.x);
        const int y = blockData->getY();
        const int z = convertCoordIntoChunkCoords(blockData->getZ(), chunkCoord.y);
        if (x == 0 || x == CHUNK_LENGTH_PLUS_2 - 1 || z == 0 || z == CHUNK_LENGTH_PLUS_2 - 1)
            continue;
        std::array<bool, 6> neighborsExist = {false}; // left, right, bottom, top, back, front
        for (int j = 0; j < 2; j++)
        {
            neighborsExist[j + 0] = getBlock(glm::vec3(x + modifiers[j], y, z)).has_value();
            if (y + modifiers[j] >= 0 && y + modifiers[j] < CHUNK_HEIGHT)
                neighborsExist[j + 2] = getBlock(glm::vec3(x, y + modifiers[j], z)).has_value();
            neighborsExist[j + 4] = getBlock(glm::vec3(x, y, z + modifiers[j])).has_value();
        }
        BlockMesh blockMesh(blockData.value(), neighborsExist);
        addBlockMesh(blockMesh);
    }
}

// could be opti probably
void ChunkMesh::addBlockMesh(const BlockMesh &blockMesh)
{
    size_t nbVerticesInChunkMesh = vertices.size() / 5;
    for (size_t i = 0; i < blockMesh.nbVertices(); i++)
        vertices.push_back(blockMesh.getVertex(i));
    for (size_t i = 0; i < blockMesh.nbFaces(); i++)
        faces.push_back(nbVerticesInChunkMesh + blockMesh.getFace(i));
}

std::vector<float> ChunkMesh::getVertices() const
{
    return vertices;
}
std::vector<unsigned int> ChunkMesh::getFaces() const
{
    return faces;
}