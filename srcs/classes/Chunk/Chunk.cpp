#include "Chunk.hpp"
#include "../../../libs/glad/include/glad/glad.h"
#include <memory>

Chunk::Chunk()
{
    init = false;
}

Chunk::~Chunk()
{
    if (init)
        unloadChunk();
}

unsigned int Chunk::getVAO() const
{
    return (VAO);
}

void Chunk::loadChunk()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    updateChunk();

    init = true;
}

#include <iostream>
void Chunk::updateChunk()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    convertBlocks();
    // 6 faces, 4 vertices for 1 face (a square), 5 elements by vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * blocks.size() * 4 * 5 * 6, &vertices[0], GL_STATIC_DRAW);
    // 6 faces, 2 triangles by faces, 3 points by triangles
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * blocks.size() * 6 * 2 * 3, &faces[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chunk::convertBlocks()
{
    int verticesIndex = 0;
    vertices = std::make_unique<float[]>(blocks.size() * 4 * 5 * 6);
    int facesIndex = 0;
    faces = std::make_unique<unsigned int[]>(blocks.size() * 6 * 2 * 3);
    for (size_t i = 0; i < blocks.size(); i++)
    {
        const std::unique_ptr<float[]> &blockVertices = blocks[i].getVertices();
        for (size_t j = 0; j < 4 * 5 * 6; j++)
        {
            vertices[verticesIndex] = blockVertices[j];
            verticesIndex++;
        }
        const std::unique_ptr<unsigned int[]> &blockFaces = blocks[i].getFaces();
        for (size_t j = 0; j < 6 * 2 * 3; j++)
        {
            faces[facesIndex] = i * 24 + blockFaces[j];
            facesIndex++;
        }
    }
}

void Chunk::unloadChunk()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    init = false;
}

std::vector<BlockData> Chunk::getBlocks() const
{
    return (blocks);
}

void Chunk::addBlock(const BlockData &block)
{
    blocks.push_back((block));
}