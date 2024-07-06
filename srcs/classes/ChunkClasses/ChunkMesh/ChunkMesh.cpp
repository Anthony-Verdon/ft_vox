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
        for (int i = 0; i < MeshType::MESH_TYPE_COUNT; i++)
        {
            vertices[i] = instance.getVertices(static_cast<MeshType>(i));
            faces[i] = instance.getFaces(static_cast<MeshType>(i));
        }
        chunkCoord = instance.getChunkCoord();
        for (unsigned int i = 0; i < CHUNK_ARRAY_SIZE; i++)
            this->blocks[i] = instance.getBlock(i);
    }
    return (*this);
}

ChunkMesh::~ChunkMesh()
{
}

void ChunkMesh::initMesh()
{
    static const std::array<int, 2> modifiers = {-1, 1};

    for (int i = 0; i < MeshType::MESH_TYPE_COUNT; i++)
    {
        vertices[i].clear();
        faces[i].clear();
    }

    for (int x = 0; x < CHUNK_LENGTH_PLUS_2; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_LENGTH_PLUS_2; z++)
            {
                BlockType type = blocks[convert3DcoordsInto1Dcoords(x, y, z)];
                if (type == BlockType::AIR)
                    continue;

                int worldPosX = x + chunkCoord.x * CHUNK_LENGTH;
                int worldPosZ = z + chunkCoord.y * CHUNK_LENGTH;
                if (x == 0 || x == CHUNK_LENGTH_PLUS_2 - 1 || z == 0 || z == CHUNK_LENGTH_PLUS_2 - 1)
                    continue;

                std::array<bool, 6> neighborsExist = {false}; // left, right, bottom, top, back, front
                for (int j = 0; j < 2; j++)
                {
                    if (type == BlockType::WATER)
                    {
                        neighborsExist[j + 0] = getBlock(glm::vec3(x + modifiers[j], y, z)) == BlockType::WATER;
                        if (y + modifiers[j] >= 0 && y + modifiers[j] < CHUNK_HEIGHT)
                            neighborsExist[j + 2] = getBlock(glm::vec3(x, y + modifiers[j], z)) == BlockType::WATER;
                        neighborsExist[j + 4] = getBlock(glm::vec3(x, y, z + modifiers[j])) == BlockType::WATER;
                    }
                    else
                    {
                        neighborsExist[j + 0] = getBlock(glm::vec3(x + modifiers[j], y, z)) != BlockType::WATER &&
                                                getBlock(glm::vec3(x + modifiers[j], y, z)) != BlockType::AIR;
                        if (y + modifiers[j] >= 0 && y + modifiers[j] < CHUNK_HEIGHT)
                            neighborsExist[j + 2] = getBlock(glm::vec3(x, y + modifiers[j], z)) != BlockType::WATER &&
                                                    getBlock(glm::vec3(x, y + modifiers[j], z)) != BlockType::AIR;
                        neighborsExist[j + 4] = getBlock(glm::vec3(x, y, z + modifiers[j])) != BlockType::WATER &&
                                                getBlock(glm::vec3(x, y, z + modifiers[j])) != BlockType::AIR;
                    }
                }
                BlockMesh blockMesh(worldPosX, y, worldPosZ, BlockDico::getBlockData(type), neighborsExist);
                addBlockMesh(blockMesh);
            }
        }
    }
}

//@todo check if it could be opti (probably)
//@todo check if it can be done a better way than a if/else because I will probably need to add a third mesh for
// transparent but with a opacity at 100% like glass
void ChunkMesh::addBlockMesh(const BlockMesh &blockMesh)
{
    MeshType type;
    if (blockMesh.getIsTranslucent())
        type = MeshType::TRANSLUCENT;
    else
        type = MeshType::OPAQUE;

    size_t nbVerticesInChunkMesh = vertices[type].size() / 5;
    for (size_t i = 0; i < blockMesh.nbVertices(); i++)
        vertices[type].push_back(blockMesh.getVertex(i));
    for (size_t i = 0; i < blockMesh.nbFaces(); i++)
        faces[type].push_back(nbVerticesInChunkMesh + blockMesh.getFace(i));
}

std::vector<float> ChunkMesh::getVertices(MeshType type) const
{
    return vertices[type];
}
std::vector<unsigned int> ChunkMesh::getFaces(MeshType type) const
{
    return faces[type];
}