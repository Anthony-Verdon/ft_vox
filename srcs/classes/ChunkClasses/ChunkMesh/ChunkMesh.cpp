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

void ChunkMesh::initMesh()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    convertBlocks();
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
        for (size_t i = 0; i < blockMesh.nbFaces(); i += 3)
        {
            std::vector<float> newFaceCoords(15);
            for (int j = 0; j < 3; j++)
            {
                for (unsigned int k = 0; k < 5; k++)
                    newFaceCoords[j * 5 + k] = blockMesh.getVertex(blockMesh.getFace(i + j) * 5 + k);
            }

            int faceIndex = faceIndexInMesh(newFaceCoords);
            if (faceIndex != -1)
                faces.erase(faces.begin() + faceIndex, faces.begin() + faceIndex + 3);
            else
                addFaceToMesh(newFaceCoords);
        }
    }
}

int ChunkMesh::faceIndexInMesh(const std::vector<float> &newFaceCoords)
{
    // newFaceCoords has a size of 15,
    // so this part (k / 3 * 2) is to always check the coord xyz and not compare to uv
    bool alreadyExist = false;
    size_t j;
    for (j = 0; j < faces.size(); j += 3)
    {
        std::vector<float> faceCoords = findFaceCoords(j);
        alreadyExist = true;
        for (int k = 0; k < 9; k++)
        {
            if (faceCoords[k] != newFaceCoords[k / 3 * 2 + k])
            {
                alreadyExist = false;
                break;
            }
        }
        if (alreadyExist)
            break;
    }
    if (alreadyExist)
        return (j);
    else
        return (-1);
}

void ChunkMesh::addFaceToMesh(const std::vector<float> &newFaceCoords)
{
    for (int k = 0; k < 3; k++)
    {
        int vertexIndex = vertexIndexInMesh(newFaceCoords.begin() + k * 5, newFaceCoords.begin() + (k + 1) * 5);
        if (vertexIndex == -1)
        {
            for (unsigned int j = 0; j < 5; j++)
                vertices.push_back(newFaceCoords[k * 5 + j]);
            faces.push_back((vertices.size() / 5 - 1));
        }
        else
            faces.push_back((vertexIndex / 5));
    }
}

int ChunkMesh::vertexIndexInMesh(const std::vector<float>::const_iterator &start,
                                 const std::vector<float>::const_iterator &end)
{
    auto it = vertices.begin();
    while (it != vertices.end())
    {
        it = std::search(it, vertices.end(), start, end);
        if (it != vertices.end())
        {
            unsigned int distance = std::distance(vertices.begin(), it++);
            if (distance % 5 == 0)
                return (distance);
        }
    }
    return -1;
}

std::vector<float> ChunkMesh::findFaceCoords(unsigned int faceStartIndex) const
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