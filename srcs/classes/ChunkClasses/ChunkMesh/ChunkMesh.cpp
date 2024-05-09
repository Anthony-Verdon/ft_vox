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
        coordX = instance.getX();
        coordZ = instance.getZ();
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

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::initMesh(const std::array<std::optional<ChunkData>, 4> &neighborsChunks)
{
    faces.clear();
    vertices.clear();
    const std::array<int, 2> modifiers = {-1, 1};
    for (int i = 0; i < CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH; i++)
    {
        std::optional<BlockData> blockData = blocks[i];
        if (!blockData.has_value())
            continue;
        const int x = convertCoordIntoChunkCoords(blocks[i]->getX());
        const int y = blocks[i]->getY();
        const int z = convertCoordIntoChunkCoords(blocks[i]->getZ());
        std::array<bool, 6> neighborsExist = {false}; // left, right, bottom, top, back, front
        for (int j = 0; j < 2; j++)
        {
            if (x + modifiers[j] >= 0 && x + modifiers[j] < CHUNK_LENGTH)
                neighborsExist[j + 0] = getBlock(x + modifiers[j], y, z).has_value();
            else if (neighborsChunks[j + 0].has_value())
            {
                if (j == 0)
                    neighborsExist[j + 0] = neighborsChunks[j + 0]->getBlock(CHUNK_LENGTH - 1, y, z).has_value();
                else
                    neighborsExist[j + 0] = neighborsChunks[j + 0]->getBlock(0, y, z).has_value();
            }
            if (y + modifiers[j] >= 0 && y + modifiers[j] < CHUNK_HEIGHT)
                neighborsExist[j + 2] = getBlock(x, y + modifiers[j], z).has_value();

            if (z + modifiers[j] >= 0 && z + modifiers[j] < CHUNK_LENGTH)
                neighborsExist[j + 4] = getBlock(x, y, z + modifiers[j]).has_value();
            else if (neighborsChunks[j + 2].has_value())
            {
                if (j == 0)
                    neighborsExist[j + 4] = neighborsChunks[j + 2]->getBlock(x, y, CHUNK_LENGTH - 1).has_value();
                else
                    neighborsExist[j + 4] = neighborsChunks[j + 2]->getBlock(x, y, 0).has_value();
            }
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