#include "ChunkMesh.hpp"
#include "../../../../libs/glad/include/glad/glad.h"
#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include <algorithm>
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

#include <iostream>
void ChunkMesh::initMesh()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    convertBlocks();
    eraseSimilarFaces();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    isInit = true;
}

void ChunkMesh::convertBlocks()
{
    for (int b = 0; b < CHUNK_LENGTH * CHUNK_HEIGHT * CHUNK_LENGTH; b++)
    {
        std::optional<BlockData> blockData = blocks[b];
        if (!blockData.has_value())
            continue;
        BlockMesh blockMesh(blockData.value());
        for (size_t i = 0; i < blockMesh.nbFaces() / 3; i++)
        {
            for (int k = 0; k < 3; k++)
            {
                std::vector<float> vertex(5);
                for (unsigned int j = 0; j < 5; j++)
                    vertex[j] = blockMesh.getVertex(blockMesh.getFace(i * 3 + k) * 5 + j);
                int vertexIndex = vertexIndexInMesh(vertex);
                if (vertexIndex == -1)
                {
                    for (unsigned int j = 0; j < 5; j++)
                        vertices.push_back(vertex[j]);
                    faces.push_back((vertices.size() / 5 - 1));
                }
                else
                    faces.push_back((vertexIndex / 5));
            }
        }
    }
}

int ChunkMesh::vertexIndexInMesh(const std::vector<float> &vertex)
{
    auto it = vertices.begin();
    while (it != vertices.end())
    {
        it = std::search(it, vertices.end(), vertex.begin(), vertex.end());
        if (it != vertices.end())
        {
            unsigned int distance = std::distance(vertices.begin(), it++);
            if (distance % 5 == 0)
                return (distance);
        }
    }
    return -1;
}

void ChunkMesh::eraseSimilarFaces()
{
    for (size_t i = 0; i < faces.size(); i += 3)
    {
        std::vector<float> coordsSearched = faceCoords(i);
        std::vector<unsigned int> indexFound;
        for (size_t j = 0; j < faces.size(); j += 3)
        {
            std::vector<float> coordsHere = faceCoords(j);
            if (coordsSearched == coordsHere)
                indexFound.push_back(j);
        }
        if (indexFound.size() <= 1)
            continue;
        std::sort(indexFound.begin(), indexFound.end());
        for (int j = indexFound.size() - 1; j >= 0; j--)
            faces.erase(faces.begin() + indexFound[j], faces.begin() + indexFound[j] + 3);
    }
}

std::vector<float> ChunkMesh::faceCoords(unsigned int faceStartIndex) const
{
    std::vector<float> coords(9);
    for (int j = 0; j < 3; j++)
    {
        int vertexIndex = faces[faceStartIndex + j];
        for (int k = 0; k < 3; k++)
            coords[j * 3 + k] = vertices[vertexIndex * 5 + k];
    }
    return (coords);
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