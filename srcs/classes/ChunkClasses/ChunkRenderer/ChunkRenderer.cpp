#include "ChunkRenderer.hpp"
#include "../../../../libs/glad/include/glad/glad.h"

ChunkRenderer::ChunkRenderer(const ChunkMesh &chunkMesh) : ChunkMesh(chunkMesh)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    initRenderer();
}

/*
ChunkRenderer::ChunkRenderer(const ChunkRenderer &instance) : ChunkMesh(instance)
{
    *this = instance;
}

ChunkRenderer &ChunkRenderer::operator=(const ChunkRenderer &instance)
{
    if (this != &instance)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        initRenderer();
        // todo: copy
    }
    return (*this);
}
*/
ChunkRenderer::~ChunkRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ChunkRenderer::initRenderer()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

unsigned int ChunkRenderer::getVAO() const
{
    return (VAO);
}
