#include "ChunkMesh.hpp"
#include "../../../../libs/glad/include/glad/glad.h"
#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include <cstdlib>
#include <iostream>

ChunkMesh::ChunkMesh(const ChunkData &data) : ChunkData(data)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    init = false;
}

ChunkMesh::ChunkMesh(const ChunkMesh &instance)
{
    *this = instance;
}

ChunkMesh &ChunkMesh::operator=(const ChunkMesh &instance)
{
    if (this != &instance)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        vertices = instance.getVertices();
        faces = instance.getFaces();
        init = instance.isInit();
        x = instance.getX();
        z = instance.getZ();
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
        // if (init)
        //     initMesh();
    }
    return (*this);
}

ChunkMesh::~ChunkMesh()
{
    if (init)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void ChunkMesh::initMesh(const std::array<std::optional<ChunkData>, 4> &neighborsChunks)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    convertChunkDataIntoMesh(neighborsChunks);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    init = true;
}

void ChunkMesh::convertChunkDataIntoMesh(const std::array<std::optional<ChunkData>, 4> &neighborsChunks)
{
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

unsigned int ChunkMesh::getVAO() const
{
    return VAO;
}
std::vector<float> ChunkMesh::getVertices() const
{
    return vertices;
}
std::vector<unsigned int> ChunkMesh::getFaces() const
{
    return faces;
}

bool ChunkMesh::isInit() const
{
    return (init);
}