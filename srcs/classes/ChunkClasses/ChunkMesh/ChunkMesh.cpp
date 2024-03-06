#include "ChunkMesh.hpp"
#include "../../../../libs/glad/include/glad/glad.h"
#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
ChunkMesh::ChunkMesh(const ChunkData &data) : ChunkData(data)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    isInit = false;
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
        for (int x = 0; x < CHUNK_LENGTH; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_LENGTH; z++)
                {
                    this->blocks[x * CHUNK_LENGTH + y * CHUNK_HEIGHT + z] = instance.getBlock(x, y, z);
                }
            }
        }
    }
    isInit = false;
    return (*this);
}

ChunkMesh::~ChunkMesh()
{
    if (isInit)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void ChunkMesh::initMesh()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    convertBlocks();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 5, vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size() * 3, faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    isInit = true;
}

void ChunkMesh::convertBlocks()
{
    int nbBlock = 0;
    for (int x = 0; x < CHUNK_LENGTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_LENGTH; z++)
            {
                std::optional<BlockData> blockData = blocks[x * CHUNK_LENGTH + y * CHUNK_HEIGHT + z];
                if (!blockData.has_value())
                    continue;
                BlockMesh blockMesh(blockData.value());
                for (size_t i = 0; i < NB_VERTICES * 5; i++)
                    vertices.push_back(blockMesh.getVertex(i));
                for (int i = 0; i < NB_FACES * 2 * 3; i++)
                    faces.push_back(nbBlock * 24 + blockMesh.getFace(i));
                nbBlock++;
            }
        }
    }
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